#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include <dirent.h>
#include <sys/wait.h>	         
#include <sys/socket.h>	    
#include <netinet/in.h>	     
#include <netdb.h>
#include <pthread.h>	         
#include <ctype.h>	       
#include "bloom.h"
#include "data.h"
#include "threads.h"

extern int errno;
#define PERMS   0666

#define my_perror(s,e) fprintf(stderr,"%s: %s\n",s, strerror(e))

pthread_mutex_t thread_mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t condition_mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition_full;
pthread_cond_t condition_empty;

int END = 0;
int count_signals=0;

void *get_file(void * node){
    int kati=0;
    thread_args* x = node;
   
    while(1){
        
        pthread_mutex_lock(&condition_mutex);
        
        while(x->file_buffer_node->counter <= 0){// IF THE BUFFER IS EMPTY WAIT
            count_signals++;
            pthread_cond_signal(&condition_empty);
            pthread_cond_wait(&condition_full, &condition_mutex);
        }

        pthread_mutex_unlock(&condition_mutex);
        
        if(END==1){//EXIT
            pthread_exit(0);
        }

        pthread_mutex_lock(&thread_mutex);

        if(x->file_buffer_node->counter > 0){//IF THE BUFFER IS NOT EMPTY
        
            put_records_in_structurs(&(x->first_data), &(x->last_data), &(x->first_bloom), &(x->last_bloom), 
                                        x->file_buffer_node->file[x->file_buffer_node->first], x->bloomSize );

            x->file_buffer_node->first=(x->file_buffer_node->first +1) % x->file_buffer_node->size;
            x->file_buffer_node->counter--;
        }
            
        pthread_mutex_unlock(&thread_mutex);

        usleep(500000);
        
    }
    
}

int main(int argc, char** argv){
  
    int free_h=0;
    int total = 0, accepted = 0, rejected = 0;
    int K = 16;

    thread_args* args_node = malloc(sizeof(thread_args));//CREATE NODE WITH ARGS FOR THREADS

    data_node* new1 = malloc(sizeof(data_node));//CREATE LIST OF DATA
    new1->next = NULL;
    new1->citizenID = "-1"; 
    args_node->first_data = new1;
    args_node->last_data = new1;

    bloom_node* new = malloc(sizeof(bloom_node));//CREATE LIST OF BLOOM FILTERS
    new->v_name= malloc(3);
    strcpy(new->v_name, "-1");
    new->bloom= NULL;
    new->next = NULL;
    args_node->first_bloom = new;
    args_node->last_bloom = new;

    country_files* new2 = malloc(sizeof(country_files));//CREATE LIST OF COUNTRY FILES
    country_files* first_file = new2;
    country_files* last_file = new2;
    new2->next = NULL;
    new2->file = "-1"; 
    

    int error;
    int port = atoi(argv[2]);
    int num_threads = atoi(argv[4]);
    int MAXBUF = atoi(argv[6]);
    int cyclicbuffersize = atoi(argv[8]);
    int bloomSize = atoi(argv[10]);
    char buf[MAXBUF];
    char* dire=malloc(strlen(argv[11])+10);
    strcpy(dire,argv[11]);
    char* dire2 = strtok(dire, "/");
    char* directory = strtok(NULL, "/");
    args_node->bloomSize=bloomSize;

    pthread_mutex_init(&condition_mutex, 0);
    pthread_cond_init(&condition_empty,0);
    pthread_cond_init(&condition_full,0);

    int  my_socket, acc_socket;
    struct sockaddr_in server, client;
    struct sockaddr *server_ptr = (struct sockaddr *)&server;
    struct sockaddr *client_ptr = (struct sockaddr *)&client;
    socklen_t clientlength = sizeof(client);
    

    if ((my_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) //CREATE SOCKET
        perror("socket");

    u_int port_rep = 1;
    setsockopt(my_socket, SOL_SOCKET, SO_REUSEADDR, &port_rep, sizeof(port_rep)); //USE SAME PORT

    server.sin_family = AF_INET;      
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port); 
    
    if (bind(my_socket, server_ptr, sizeof(server)) < 0)//BIND SOCKET TO ADDRESS
        perror("bind");
        
    if (listen(my_socket, 5) < 0)//LISTEN FOR CONNECTION
        perror("listen");
   
    if ((acc_socket = accept(my_socket, client_ptr, &clientlength)) < 0)//ACCEPT CONNECTION
        perror("accept");
   
    
    int ptr=11;
    DIR *dp;
    struct dirent *dir;

    char ** file_array = NULL;
    int total_num_of_files=0;

    while(argv[ptr]!=NULL){//FOR EVERY COUNTRY PATH
        
        if((dp=opendir(argv[ptr]))==NULL){ // OPEN COUNTRY FILE DIRECTORY 
            perror("Opendir:");
        }

      
        while((dir=readdir(dp))!=NULL){ //FOR EVERY .TXT FILE IN CURRENT COUNTRY
         	if(dir->d_type ==DT_REG){
                
                file_array = realloc(file_array, (total_num_of_files+1)*sizeof(*file_array));//ARRAY WITH ALL THE COUNTRY.TXT PATHS 
                file_array[total_num_of_files] = malloc(strlen(argv[ptr])+(strlen(dir->d_name) +3)* sizeof(char*));
                sprintf(file_array[total_num_of_files],"%s/%s",argv[ptr] ,dir->d_name); 
                total_num_of_files++;
                push_country_files(&first_file,&last_file,dir->d_name);
            }
            
        }

        ptr++;
        closedir(dp);
    }

    
    file_buffer* new_node=malloc(sizeof(file_buffer));
    initialize_file_buffer(new_node,cyclicbuffersize);
    args_node->file_buffer_node =new_node;

    int current_files_num=0;
    if(total_num_of_files<cyclicbuffersize)//THE FIRST NUMBER OF PATHS FOR THE BUFFER
        current_files_num=total_num_of_files;
    else current_files_num=cyclicbuffersize;
        
        
    for(int i=0; i<current_files_num; i++)//ADD PATHS
        add_file(args_node->file_buffer_node,file_array[i]); 
    
    pthread_t thread_array[num_threads];

    for(int i=0; i<num_threads; i++){//CREATE THREADS

        pthread_create(&thread_array[i], NULL, get_file, (void*) args_node);
    }

    int remain_files_num = total_num_of_files - cyclicbuffersize;//REMAIN PATHS

    while(remain_files_num>0){//UNTILE WE PUT ALL THE PATHS IN THE BUFFER
        
        while(args_node->file_buffer_node->counter>0 || count_signals<num_threads){//IF THE BUFFER IS FULL WAIT
            pthread_cond_wait(&condition_empty, &condition_mutex);
        }
    
        pthread_mutex_unlock(&condition_mutex);
        pthread_mutex_lock(&condition_mutex);

        for(int i=0; i<cyclicbuffersize; i++)//CLEAN THE BUFFER AND ADD NEW PATHS
            free(args_node->file_buffer_node->file[i]);
        free(args_node->file_buffer_node->file);

        initialize_file_buffer(new_node,cyclicbuffersize);

        current_files_num=0;
        if(remain_files_num<cyclicbuffersize)//THE NEXT NUMBER OF PATHS FOR THE BUFFER
            current_files_num=remain_files_num;
        else current_files_num=cyclicbuffersize;
        
        int start_from=total_num_of_files-remain_files_num;
        for(int i=start_from; i<(start_from+current_files_num); i++)
            add_file(args_node->file_buffer_node,file_array[i]); 

        pthread_cond_broadcast(&condition_full);
        pthread_mutex_unlock(&condition_mutex);

        count_signals=0;
        remain_files_num=remain_files_num-current_files_num;//REMAIN PATHS
    }

    while(args_node->file_buffer_node->counter>0 || count_signals<num_threads){//IF BUFFER IS FULL WAIT
        pthread_cond_wait(&condition_empty, &condition_mutex);
    }


    bloom_node* current=args_node->first_bloom; 
    while(current!=NULL){// FOR EVERY VIRUS AND BLOOM FILTER
    
        char* msg=malloc(strlen(current->v_name)+bloomSize+10); //CREATE STRING WITH VIRUS AND BLOOM FILTER
        strcpy(msg,current->v_name);
        strcat(msg," ");
        strcat(msg,current->bloom);
        strcat(msg," ");
     

        int i=0;
        while(i<strlen(msg)){//WRITE VIRUS AND BLOOM FILTERS
           
            strncpy(buf,&msg[i],MAXBUF); 
            i=i+MAXBUF;
            
            if (write(acc_socket, buf, MAXBUF) < 0)
                perror("Monitor: data write error \n");
            
           
        }

        free(msg);
        current=current->next;
    }
    
    if(write(acc_socket, "#", 1) < 0)
        perror("Monitor: data write error \n");
  
  

    //FREE FILE ARRAY
    
    for(int z=0; z<total_num_of_files;z++)
        free(file_array[z]);
             
    free(file_array);
   
    
    while(1){
       
        char* request;
        int x;
       
        if((x=read(acc_socket, buf, MAXBUF))<0){ //READ MAXBUF LETTERS
                perror("Monitor: data read error \n");
        }
        
            
        char* prev=malloc(2);
        strcpy(prev," ");
        while ( buf[0]!='#'){ // READ THE REQUEST UNTILE YOU GET #
            
            buf[x]='\0';

            char* next=malloc(strlen(prev)+strlen(buf)+1); 
            strcpy(next,prev);
            strcat(next,buf);
            free(prev);

            prev=malloc(strlen(next)+1);
            strcpy(prev,next);
            free(next);

            if((x=read(acc_socket, buf, MAXBUF))<0){ //READ MAXBUF LETTERS
                perror("Monitor: data read error \n");
            }

        }
            
        request = strtok(prev, " ");
        
        if(strcmp(request,"/travelRequest")==0){//-------------------------- /travelRequest ------------------------------
         
            char* ID = strtok(NULL, " ");
            char* date = strtok(NULL, " ");
            char* countryFrom = strtok(NULL, " ");
            char* countryTo = strtok(NULL, " ");
            char* virusName = strtok(NULL, "\n");
            char* ans;
            travelRequest(args_node->first_data,ID,virusName,&ans,&accepted,&rejected);
     
            int j=0;
            while(j<strlen(ans)){//WRITE ANSWER
               
                strncpy(buf,&ans[j],MAXBUF);  
                j=j+MAXBUF;
            
                if(write(acc_socket, buf, MAXBUF) < 0)
                    perror("Monitor: data write error \n");
   
            }

            if(write(acc_socket, "#", 1) < 0)
                perror("Monitor: data write error \n");


            free(prev);
    
        }else if(strcmp(request,"/addVaccinationRecords")==0){//----------------------------- /addVaccinationRecords -----------------------------------
            

            char* country = strtok(NULL, "\n");
         
            char country_files_dir[strlen(directory)+strlen(country)+5];
            sprintf(country_files_dir, "./%s/%s",directory,country);
           
            if((dp=opendir(country_files_dir))==NULL){ // OPEN COUNTRY FILE DIRECTORY
                perror("Opendir:");
            }
            char ** file_array2 = NULL;
            int total_num_of_files2=0;
            
            int i=1;
            while((dir=readdir(dp))!=NULL){ //FOR EVERY .TXT FILE IN THE CURRENT COUNTRY

                if(dir->d_type ==DT_REG){

                    if(push_country_files(&first_file,&last_file,dir->d_name)==-1){// IF THE CURRENT COUNTRY FILE ALREADY EXIST CONTINUE
                        continue;
                    }
                    char final_name[strlen(country_files_dir)+strlen(dir->d_name)+2];
                    sprintf(final_name, "%s/%s",country_files_dir,dir->d_name); //FINAL PATH

                    file_array2 = realloc(file_array2, (total_num_of_files2+1)*sizeof(*file_array2));
                    file_array2[total_num_of_files2]= malloc((strlen(final_name)+3)* sizeof(char*));
                    strcpy(file_array2[total_num_of_files2],final_name); 
                    total_num_of_files2++;
                    push_country_files(&first_file,&last_file,dir->d_name);  
                }
                    
            }
                
            closedir(dp);

            int remain_files_num = total_num_of_files2 ;//NUMBER OF THE NEW TXT

            while(remain_files_num>0){//UNTILE WE PUT ALL THE PATHS IN THE BUFFER
              
                count_signals=0;
                pthread_mutex_unlock(&condition_mutex);
                pthread_mutex_lock(&condition_mutex);

                for(int i=0; i<current_files_num; i++)//CLEAN THE BUFFER AND ADD THE NEW PATHS
                    free(args_node->file_buffer_node->file[i]);
                
                free(args_node->file_buffer_node->file);
                initialize_file_buffer(new_node,cyclicbuffersize);

                current_files_num=0;
                if(remain_files_num<cyclicbuffersize)//THE NEXT NUMBER OF PATHS FOR THE BUFFER
                    current_files_num=remain_files_num;
                else current_files_num=cyclicbuffersize;
            
                int start_from=total_num_of_files2-remain_files_num;
                for(int i=start_from; i<(start_from+current_files_num); i++)
                    add_file(args_node->file_buffer_node,file_array2[i]); 

                pthread_cond_broadcast(&condition_full);
                pthread_mutex_unlock(&condition_mutex);

                while(args_node->file_buffer_node->counter>0 || count_signals<num_threads){//IF THE BUFFER IS FULL WAIT
                    pthread_cond_wait(&condition_empty, &condition_mutex);
                }
            
                remain_files_num=remain_files_num-current_files_num;
            }
            free_h=current_files_num;
        
            for(int z=0; z<total_num_of_files2; z++){
                free(file_array2[z]);
             
            }   
            free(file_array2);

            bloom_node* current=args_node->first_bloom; 
            while(current!=NULL){// FOR EVERY VIRUS AND BLOOM FILTER
                char* msg=malloc(strlen(current->v_name)+bloomSize+10); //CREATE STRING WITH VIRUS AND BLOOM FILTER
                strcpy(msg,current->v_name);
                strcat(msg," ");
                strcat(msg,current->bloom);
                strcat(msg," ");

                int i=0;
                while(i<strlen(msg)){//WRITE VIRUS AND BLOOM FILTERS
                
                    strncpy(buf,&msg[i],MAXBUF); 
                    i=i+MAXBUF;
                    
                    if (write(acc_socket, buf, MAXBUF) < 0)
                        perror("Monitor: data write error \n");
                }

                free(msg);
                current=current->next;
            }
            
            if(write(acc_socket, "#", 1) < 0)
                perror("Monitor: data write error \n");
            
            free(prev);
     
        }else if(strcmp(request,"/searchVaccinationStatus")==0){// ---------------------------- /searchVaccinationStatus ------------------------
            
            char* ID = strtok(NULL, "\n");
            char* output;

            if(SearchVaccinationStatus(args_node->first_data,ID,&output)!=-1){//IF THE ID EXIST

                int j=0;
                while(j<strlen(output)){//WRITE ANSWER
                
                    strncpy(buf,&output[j],MAXBUF);  
                    j=j+MAXBUF;
                
                    if(write(acc_socket, buf, MAXBUF) < 0)
                        perror("Monitor: data write error \n");
    
                }

                if(write(acc_socket, "#", 1) < 0)
                    perror("Monitor: data write error \n");

            }else{// IF THE ID DOESNT EXIST

                if(write(acc_socket, "#", 1) < 0)
                    perror("Monitor: data write error \n");
            }
        
            free(prev);
        }else if(strcmp(request,"/exit\n")==0){ 

            int ptr2=11;
            char* country_string=malloc(2);
            strcpy(country_string," ");
            int country_string_len=2;
            while(argv[ptr2]!=NULL){//CREATE STRING WITH ALL THE COUNTRIES

                char* dire24 = strtok(argv[ptr2], "/");
                char* directory = strtok(NULL, "/");
                char* country = strtok(NULL, " ");

                country_string = realloc(country_string, (country_string_len+strlen(country+1)));
                strcat(country_string,country);
                strcat(country_string," ");

                country_string_len+=strlen(country+1);
                ptr2++;
            
            }
          
            country_string[country_string_len]='\0';
            logfile(accepted,rejected,country_string);

            pthread_mutex_unlock(&condition_mutex);//EXIT THREADS 
            pthread_mutex_lock(&condition_mutex);

            END = 1;
            args_node->file_buffer_node->counter=2;   
            pthread_cond_broadcast(&condition_full);
            pthread_mutex_unlock(&condition_mutex);


            for(int i=0; i<num_threads; i++){
                if(error =pthread_join(thread_array[i], NULL)){
                    my_perror("pthread_join",error);
                    exit(0);
                }
            }
            if(error = pthread_cond_destroy(&condition_full)){
                    my_perror("pthread_cond_destroy",error);
                    exit(0);
            }
            if(error = pthread_cond_destroy(&condition_empty)){
                    my_perror("pthread_cond_destroy",error);
                    exit(0);
            }

            if(error = pthread_mutex_destroy(&thread_mutex)){
                    my_perror("pthread_mutex_destroy",error);
                    exit(0);
            }

            //FREE COUNTRY LIST
            country_files* c_current=first_file;
            first_file=NULL;
            last_file=NULL;

            while(c_current!=NULL){
                country_files* c_temp=c_current;
                c_current=c_current->next;

                free(c_temp->file);
                c_temp->next=NULL;
                free(c_temp);  
            }

            //FREE DATA LIST
            data_node* d_current=args_node->first_data;
            args_node->first_data=NULL;
            args_node->last_bloom=NULL;

            while(d_current!=NULL){
                
                data_node* d_temp=d_current;
                d_current=d_current->next;

                free(d_temp->citizenID);
                free(d_temp->firstName);
                free(d_temp->lastName);
                free(d_temp->country);
                free(d_temp->age);
                
                virus* v_current=d_temp->v_name;

                d_temp->v_name=NULL;
                while(v_current!=NULL){
                    
                    virus* v_temp=v_current;
                    v_current=v_current->next;

                    free(v_temp->date);
                    free(v_temp->virusName);
                    free(v_temp->y_n);
                    v_temp->next=NULL;
                    free(v_temp);
                }
                d_temp->next=NULL;
                free(d_temp);  
            }

            //FREE BLOOM FILTER LIST

            bloom_node* b_current=args_node->first_bloom;
            args_node->first_bloom=NULL;
            args_node->last_bloom=NULL;
        
            while(b_current!=NULL){
                
                bloom_node* b_temp=b_current;
                b_current=b_current->next;
                
                free(b_temp->bloom);
                free(b_temp->v_name);
                b_temp->next=NULL;

                free(b_temp);  
            }

            //FREE FILE BUFFER
            for(int i=0; i<current_files_num; i++)
                free(args_node->file_buffer_node->file[i]);
                
            free(args_node->file_buffer_node->file);

            //FREE ARGS NODE
            free(args_node->file_buffer_node);
            free(args_node);

            free(prev);
            free(dire);

            close(acc_socket);
          
            exit(0);
        }

    }
   
}
   
