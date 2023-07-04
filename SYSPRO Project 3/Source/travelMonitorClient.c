#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <wait.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>	         
#include <sys/socket.h>	    
#include <netinet/in.h>	     
#include <netdb.h>	         
#include <ctype.h>
#include "tmonitor_bloom.h"
#include "bloom.h"

extern int errno;

#define PERMS   0666



int q_sort_compare(const void *x, const void *y){
    return( strcmp(*(const char **)x,*(const char **)y) );
}
int main(int argc, char** argv){
   
    if(argc!=13){
        printf("WRONG ARGUMENTS\n");
        return -1;
    }

    int num_monitor =atoi(argv[2]);
    int MAXBUF= atoi(argv[4]);
    int re[num_monitor], wr[num_monitor];
    char buf[MAXBUF];
    int pid_array[num_monitor];
    int bloomSize =atoi(argv[8]);
    int K=16;
    int socket_array[num_monitor];
    int port = 9000;
    char port_s[30];
    sprintf(port_s,"%d",port);
    int num_threads = atoi(argv[12]);
    int cyclicbuffersize = atoi(argv[6]);
    int arg_counter;


   
    if(num_monitor<1 || MAXBUF<1 || bloomSize<1 || cyclicbuffersize<1 || num_threads<1){
        printf("WRONG ARGUMENTS\n");
        return -1;
    }

    monitors_bloom* new1=malloc(sizeof(monitors_bloom)); //CREATE LIST OF MONITORS_BLOOM
    monitors_bloom* first_monitor_bloom=new1;
    monitors_bloom* last_monitors_bloom=new1;
    new1->next=NULL;
    new1->monitor=-1; 

    request_node* new=malloc(sizeof(request_node));//CREATE LIST OF REQUEST
    request_node* first_request=new;
    request_node* last_request=new;
    new->f_date=NULL;
    new->next=NULL;
    new->country="-1"; 

    struct sockaddr_in server;
    struct sockaddr *server_ptr = (struct sockaddr*)&server;
    struct hostent *rem;

    DIR *dp;
    struct dirent *dir;
    int j=0;

    if((dp=opendir(argv[10]))==NULL){ // OPEN input_dir
        perror("Opendir:");
        exit(0);
    }
    char ** dir_array = NULL;
    int dir_counter=0;

    while((dir=readdir(dp))!=NULL){ //READ COUNTRIES FROM DIRECTORY
        
        int i=0;
        if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0){ //SKIP . ..
            continue;
        }
	
        dir_array = realloc(dir_array, (dir_counter+1)*sizeof(*dir_array));//ARRAY WITH ALL THE DIRECTORIES
        dir_array[dir_counter] = malloc((strlen(dir->d_name) +1)* sizeof(char*));
        strcpy(dir_array[dir_counter], dir->d_name); 
        dir_counter++;

    }

    if(num_monitor>dir_counter){
        num_monitor=dir_counter;
    }

    qsort(dir_array,dir_counter,sizeof(char *),q_sort_compare);

    char ** arg_array = malloc(11* sizeof(char*));//ARRAY WITH ARGS FOR THE MONITOR SERVERS
    arg_array[0]=malloc(10); strcpy(arg_array[0],"./monitorServer");
    arg_array[1]=malloc(3); strcpy(arg_array[1],"-p");
    arg_array[2]=malloc(strlen(port_s)+1); strcpy(arg_array[2],port_s);
    arg_array[3]=malloc(3); strcpy(arg_array[3],"-t");
    arg_array[4]=malloc(strlen(argv[12])+1); strcpy(arg_array[4],argv[12]);
    arg_array[5]=malloc(3); strcpy(arg_array[5],"-b");
    arg_array[6]=malloc(strlen(argv[4])+1); strcpy(arg_array[6],argv[4]);
    arg_array[7]=malloc(3); strcpy(arg_array[7],"-c");
    arg_array[8]=malloc(strlen(argv[6])+1); strcpy(arg_array[8],argv[6]);
    arg_array[9]=malloc(3); strcpy(arg_array[9],"-s");
    arg_array[10]=malloc(strlen(argv[8])+1); strcpy(arg_array[10],argv[8]);

    for(int i=1; i<=num_monitor; i++){ //CREATE SOCKETS AND MONITOR

        arg_counter=11;
        int z=i;
        while(z<=dir_counter){//ADD THE COUNTRY PATHS IN ARG ARRAY
            arg_array = realloc(arg_array, (arg_counter+1)*sizeof(*arg_array));
            arg_array[arg_counter] = malloc((strlen(dir_array[z-1])+1)* sizeof(char*));
            sprintf(arg_array[arg_counter],"./%s/%s",argv[10], dir_array[z-1]); 
            push_country(&first_monitor_bloom,&last_monitors_bloom,i-1,dir_array[z-1]); //PUSH NODE IN MONITORS_BLOOM FOR EVERY COUNTRY
            arg_counter++;
            z+=num_monitor;
        }
       
        arg_array = realloc(arg_array, (arg_counter+1)*sizeof(*arg_array));
        arg_array[arg_counter] = NULL;


        pid_array[i-1]=fork(); //CREATE MONITORS
         
        if(pid_array[i-1]==-1){ 
            perror("Failed to fork");
            return 1;
        }
     
        if(pid_array[i-1]==0){// CHILD: EXEC
            execvp("./monitorServer",arg_array);
            perror("execvp");
        }

        if ((socket_array[i-1] = socket(AF_INET, SOCK_STREAM, 0)) < 0)//CREATE SOCKETS
            perror("socket");
        
        if ((rem = gethostbyname("localhost")) == NULL) {	
            herror("gethostbyname"); exit(1);
        }
        server.sin_family = AF_INET;       
        memcpy(&server.sin_addr, rem->h_addr, rem->h_length);
        server.sin_port = htons(port);

        int status;
        do
            status = connect(socket_array[i-1], server_ptr, sizeof(server)); //CONNECTION WITH SERVER
            
        while(status<0);
        
        for(int y=11; y< arg_counter; y++)
            free(arg_array[y]);
        
        port++;
        sprintf(port_s,"%d",port);//NEW PORT FOR EACH SERVER
        free(arg_array[2]);
        arg_array[2]=malloc(strlen(port_s)+1); strcpy(arg_array[2],port_s);
        
    }

    int x;
    
    for(int j=0; j<num_monitor;j++){ //GET VIRUS AND BLOOM FILTERS FROM EVERY MONITOR
      
        if((x=read(socket_array[j], buf, MAXBUF))<0){ //READ THE FIRST MAXBUF LETTERS
            perror("Travel Monitor: data read error");
            exit(0);
        }

        char* prev=malloc(2);
        strcpy(prev," ");

        while ( buf[0]!='#'){ // READ THE SOCKET UNTILE YOU GET #

            buf[x]='\0';

            char* next=malloc(strlen(prev)+strlen(buf)+1); 
            strcpy(next,prev);
            strcat(next,buf);
            free(prev);

            prev=malloc(strlen(next)+1);
            strcpy(prev,next);
            free(next);

            if((x=read(socket_array[j], buf, MAXBUF))<0){ //READ MAXBUF LETTERS FROM THE SOCKET
                perror("Monitor: data read error \n");
            }
            
            
        }
       
        char* virus=strtok(prev," "); //FIRST VIRUS
        char* bloom=strtok(NULL," "); //FIRST BLOOM
      	bloom[bloomSize]='\0';

        while(virus!=NULL){
            push_bloom(&first_monitor_bloom,&last_monitors_bloom,j,virus,bloom); //INSERT CURRENT VIRUS AND BLOOM FILTER IN MONITOR_BLOOM NODE
            if((virus=strtok(NULL," "))!=NULL){
                bloom=strtok(NULL," ");
		        bloom[bloomSize]='\0';
	        }
        }

      
        free(prev);
      
    }


    while(1){

        char* request;
        char*cm2;

        char cm[200];
        printf("\nGIVE INPUT: ");
        fgets(cm, 200, stdin);

        cm2=malloc(strlen(cm)+1);
        strcpy(cm2,cm);
        
        request = strtok(cm, " ");

   
      

        if(strcmp(request,"/travelRequest")==0){//-------------------------- /travelRequest --------------------------------------
  
            char* ID = strtok(NULL, " ");
            char* date = strtok(NULL, " ");
            char* countryFrom = strtok(NULL, " ");
            char* countryTo = strtok(NULL, " ");
            char* virusName = strtok(NULL, "\n");

         
            if(virusName!=NULL){

                int hash_array[K];  
                for (int i = 0 ; i < K ; i++) //GET HASH NUMBERS
                    hash_array[i]=hash_i((unsigned char*)ID, i)%bloomSize;
         
                int f=search_blooms( first_monitor_bloom, countryFrom, virusName, hash_array, K);
               
                if(f==0){ //NOT VACCINATED
                    printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
                    push_request(&first_request,&last_request,countryTo,date,virusName,0); //PUSH REQUEST DATA 

                }else{ //MAYBE
                    
                    int p=find_monitor(first_monitor_bloom,countryFrom);

                    if(p==-1){
                        printf("WRONG INPUT\n");
                        continue;
                    }

                    int j=0;
                 
                    while(j<strlen(cm2)){//SEND THE REQUEST
         
                        strncpy(buf,&cm2[j],MAXBUF);  
                        j=j+MAXBUF;
                       
                        if (write(socket_array[p], buf, MAXBUF) < 0){
                            perror("Travel Monitor: data write error \n");
                        }
                    }
           
                    if (write(socket_array[p], "#", 1) < 0){
                        perror("Travel Monitor: data write error \n");
                    }
			
                
                    if((x=read(socket_array[p], buf, MAXBUF))<0){ //READ MAXBUF LETTERS
                        perror("Travel Monitor: data read error \n");
                    }
      
                    char* prev=malloc(2);
                    strcpy(prev," ");

                    while ( buf[0]!='#'){ // READ UNTILE YOU GET #
                     
                        buf[x]='\0';

                        char* next=malloc(strlen(prev)+strlen(buf)+3); 
                        strcpy(next,prev);
                        strcat(next,buf);
                        free(prev);
                    
                        prev=malloc(strlen(next)+1);
                        strcpy(prev,next);
                        free(next);

                        if((x=read(socket_array[p], buf, MAXBUF))<0){ //READ MAXBUF LETTERS FROM THE PIPE
                            perror("Travel Monitor: data read error \n");
                        }
                    }
                 
                  
                  char* ans = strtok(prev, " ");
                
                    if(strcmp(ans,"YES")==0){ //IF THE CITIZEN IS VACCINATED

                     char* date2 = strtok(NULL, " ");
                     int check= compare_dates(date,date2);//CHECK IF DATE IS BETWEEN 6 MONTHS

                        if(check==0){//ACCEPTED
                            printf("REQUEST ACCEPTED - HAPPY TRAVELS\n");
                            push_request(&first_request,&last_request,countryTo,date,virusName,1);//PUSH REQUEST DATA

                        }else{//NEED ANOTHER VACCINATION
                            printf("REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");
                            push_request(&first_request,&last_request,countryTo,date,virusName,0);//PUSH REQUEST DATA
                        }

                    }else{//NOT VACCINATED
                        printf("REQUEST REJECTED - YOU ARE NOT VACCINATED\n");
                        push_request(&first_request,&last_request,countryTo,date,virusName,0);
                    }

            
                    free(prev);
                }
               
            }else{
               printf("\nWRONG INPUT\n");
            }
        
        }else if(strcmp(request,"/travelStats")==0){ //--------------------------------- /travelStats -----------------------------------

            char* virus = strtok(NULL, " ");
            char* date1 = strtok(NULL, " ");
            char* date2 = strtok(NULL, " ");
            char* country = strtok(NULL, "\n");

            if(date2!=NULL){
                if(country!=NULL){//WITH COUNTRY
                    if(travelStatus(first_request,country,virus,date1,date2)==-1){
                        printf("WRONG INPUT\n"); 
                    }
                }else{//WITHOUT COUNTRY
                    if(travelStatus(first_request,"-1",virus,date1,date2)==-1){
                        printf("WRONG INPUT\n"); 
                    }
                }

            }else{
                printf("WRONG INPUT\n");
            }
    
        }else if(strcmp(request,"/addVaccinationRecords")==0){ //--------------------------------------- /addVaccinationRecords --------------------
            char* country = strtok(NULL, "\n");
        
            if(country!=NULL){

                int p=find_monitor(first_monitor_bloom,country);//FIND MONITOR FROM COUNTRY
                if(p==-1){
                    printf("WRONG INPUT\n");
                    continue;
                }

                int j=0;
                 
                while(j<strlen(cm2)){//WRITE THE REQUEST
         
                    strncpy(buf,&cm2[j],MAXBUF);  
                    j=j+MAXBUF;
                    
                    if (write(socket_array[p], buf, MAXBUF) < 0)
                        perror("Travel Monitor: data write error \n");
                }

                if (write(socket_array[p], "#", 1) < 0)
                    perror("Travel Monitor: data write error \n");

            
                if((x=read(socket_array[p], buf, MAXBUF))<0){ //READ MAXBUF LETTERS FROM THE PIPE
                    perror("Travel Monitor: data read error \n");
                }
    
                char* prev=malloc(2);
                strcpy(prev," ");

                while ( buf[0]!='#'){ // READ UNTILE YOU GET #
                    
                    buf[x]='\0';

                    char* next=malloc(strlen(prev)+strlen(buf)+3); 
                    strcpy(next,prev);
                    strcat(next,buf);
                    free(prev);
                
                    prev=malloc(strlen(next)+1);
                    strcpy(prev,next);
                    free(next);

                    if((x=read(socket_array[p], buf, MAXBUF))<0){ //READ MAXBUF LETTERS
                        perror("Travel Monitor: data read error \n");
                    }
                }
                
                char* virus=strtok(prev," ");//FIRST VIRUS
                char* bloom=strtok(NULL," ");//FIRST BLOOM
            	bloom[bloomSize]='\0';
            

                while(virus!=NULL){
               
                    update_bloom(&first_monitor_bloom,&last_monitors_bloom,p,virus,bloom);//UPDATE BLOOM FILTERS
                    if((virus=strtok(NULL," "))!=NULL){

                        bloom=strtok(NULL," ");
                        bloom[bloomSize]='\0';
                    }
                }

            
                free(prev);
                printf("SUCCESSFUL ADDITION\n");
        
            }else{
                printf("WRONG INPUT\n");
            }

        }else if(strcmp(request,"/searchVaccinationStatus")==0){//-------------------------- /searchVaccinationStatus -----------------------------------------
            
            char* ID = strtok(NULL, "\n");   

            if(ID!=NULL){

                for(int i=0; i<num_monitor;i++){ //CHECK EVERY MONITOR
                    j=0;
                    while(j<strlen(cm2)){//WRITE THE REQUEST
         
                        strncpy(buf,&cm2[j],MAXBUF);  
                        j=j+MAXBUF;
                        
                        if (write(socket_array[i], buf, MAXBUF) < 0)
                            perror("Travel Monitor: data write error \n");
                    }

                    if (write(socket_array[i], "#", 1) < 0)
                        perror("Travel Monitor: data write error \n");

                }

                for(int i=0; i<num_monitor;i++){

                    
                    if((x=read(socket_array[i], buf, MAXBUF))<0){ //READ MAXBUF LETTERS
                        perror("Travel Monitor: data read error \n");
                    }
        
                    char* prev=malloc(2);
                    strcpy(prev," ");

                    while ( buf[0]!='#'){ // READ UNTILE YOU GET #
                        
                        buf[x]='\0';

                        char* next=malloc(strlen(prev)+strlen(buf)+3); 
                        strcpy(next,prev);
                        strcat(next,buf);
                        free(prev);
                    
                        prev=malloc(strlen(next)+1);
                        strcpy(prev,next);
                        free(next);

                        if((x=read(socket_array[i], buf, MAXBUF))<0){ //READ MAXBUF LETTERS FROM
                            perror("Travel Monitor: data read error \n");
                        }
                    }

                    if(strcmp(prev," ")!=0)//PRINT STATUS
                        printf("\n%s\n",&prev[1]);

                    free(prev);
                }


            }else{
                printf("WRONG INPUT\n");
            }

        }else if(strcmp(request,"/exit\n")==0){ //------------------------------- /exit ---------------------
         
            printf("EXIT\n");

            logfile(first_request, first_monitor_bloom);//CREATE LOG FILE

            for(int i=0; i<num_monitor; i++){
                int j=0;
                while(j<strlen(cm2)){//WRITE THE REQUEST
         
                    strncpy(buf,&cm2[j],MAXBUF);  
                    j=j+MAXBUF;
                    
                    if (write(socket_array[i], buf, MAXBUF) < 0)
                        perror("Travel Monitor: data write error \n");
                }

                if (write(socket_array[i], "#", 1) < 0)
                    perror("Travel Monitor: data write error \n");
            }

            closedir(dp);

            //FREE MONITOR BLOOM LIST
            monitors_bloom* current=first_monitor_bloom;
            first_monitor_bloom=NULL;
            last_monitors_bloom=NULL;

            while(current!=NULL){
                
                monitors_bloom* temp=current;
                current=current->next;
                
                countries* c_current=temp->f_country;
                temp->f_country=NULL;
                while(c_current!=NULL){
                    
                    countries* c_temp=c_current;
                    c_current=c_current->next;

                    free(c_temp->country);
                    c_temp->next=NULL;
                    free(c_temp);
                }

                bloom_filters* b_current=temp->f_bloom;
                temp->f_bloom=NULL;
                while(b_current!=NULL){
                  
                    bloom_filters* b_temp=b_current;
                    b_current=b_current->next;

                    free(b_temp->bloom);
                    free(b_temp->virus);
                    b_temp->next=NULL;
                    free(b_temp);
                } 



                temp->next=NULL;
                free(temp);  
            }

            //FREE REQUEST LIST
            request_node* r_current=first_request;
            first_request=NULL;
            last_request=NULL;

            while(r_current!=NULL){
                
                request_node* r_temp=r_current;
                r_current=r_current->next;
                
                if(r_temp->country!="-1")
                    free(r_temp->country);
                
                dates* d_current=r_temp->f_date;

                r_temp->f_date=NULL;
                while(d_current!=NULL){
                    
                    dates* d_temp=d_current;
                    d_current=d_current->next;

                    free(d_temp->date);
                    free(d_temp->virus);
                    d_temp->next=NULL;
                    free(d_temp);
                }
                r_temp->next=NULL;
                free(r_temp);  
            } 

            for(int y=0; y<11; y++)
                free(arg_array[y]);
            free(arg_array);

            for(int y=0; y<dir_counter; y++)
                free(dir_array[y]);
            free(dir_array);


            free(cm2);
            break;    

        }else{ 
            printf("WRONG INPUT\n");
        }
        free(cm2);
      
    }

    for(int j=0; j<num_monitor;j++) //WAIT 
        waitpid(pid_array[j],NULL,0);

    for(int j=0; j<num_monitor;j++) //CLOSE 
        close(socket_array[j]);

    return 1;
}
     
