#include <sys/types.h>
#include <sys/stat.h>
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
#include "bloom.h"
#include "data.h"
#include <signal.h>
extern int errno;
#define PERMS   0666

int sigusr1_flag=0;
int sigint_flag=0;

void handle_sigusr1(int sig){
   sigusr1_flag=1;
}

void handle_sigint(int sig){
   sigint_flag=1;
}

int main(int argc, char** argv){
   
    struct sigaction sa = {0};
    sa.sa_handler = &handle_sigusr1;   
    sigaction(SIGUSR1, &sa, NULL);

    struct sigaction sa2 = {0};
    sa2.sa_handler = &handle_sigint;   
    sigaction(SIGQUIT, &sa2, NULL);
    sigaction(SIGINT, &sa2, NULL);

   
   
    int re, wr;
    int total=0, accepted=0, rejected=0;
    int K=16;

    data_node* new1=malloc(sizeof(data_node));//CREATE LIST OF DATA
    data_node* first_data=new1;
    data_node* last_data=new1;
    new1->next=NULL;
    new1->citizenID="-1"; 

    bloom_node* new=malloc(sizeof(bloom_node));//CREATE LIST OF BLOOM FILTERS
    bloom_node* first_bloom=new;
    bloom_node* last_bloom=new;
    new->next=NULL;
    new->v_name="-1";

    country_files* new2=malloc(sizeof(country_files));//CREATE LIST OF COUNTRY FILES
    country_files* first_file=new2;
    country_files* last_file=new2;
    new2->next=NULL;
    new2->file="-1"; 

    if ( (wr = open(argv[1],  1))  < 0)  { //OPEN WRITE PIPE
        perror("Monitor: can't open write fifo \n");
        exit(0);
    }
   
    if ( (re = open(argv[2], 0))  < 0)  { //OPEN READ PIPE
        perror("Monitor: can't open read fifo \n");
    }


    int x;

    char md[10];
    char bs[10];
   
    if((x=read(re, md, 10))<0){ //READ PIPE FOR THE MAX_BUFFER SIZE
        perror("Monitor: data read error");
    }
 
    md[x]='\0';
    int MAXBUF =atoi(md);
    
    if((x=read(re, bs, 10))<0){ //READ PIPE FOR THE BLOOM SIZE
        perror("Monitor: data read error");
    }
    bs[x]='\0';
    int bloomSize =atoi(bs);
   
    char buf[MAXBUF];

    char input[35];
    
    if((x=read(re, input, 30))<0){ //READ PIPE FOR THE INPUT DIR
        perror("Monitor: data read error");
    }
    input[x]='\0';
    
     
    if((x=read(re, buf, MAXBUF))<0){ //READ COUNTRIES
        perror("Monitor: data read error");
    }
  
    char b[MAXBUF+2];
    char* z;
    char* prev=malloc(2);
    char* next;

    
   
    while ( buf[0]!='#'){ // READ THE PIPE UNTILE YOU GET #

        buf[x]='\0';

        next=malloc(strlen(prev)+strlen(buf)+3); 
        strcpy(next,prev);
        strcat(next,buf);
        free(prev);

        prev=malloc(strlen(next)+1);
        strcpy(prev,next);
        free(next);

        if ((x = read(re, buf, MAXBUF)) <0) {  //READ MAXBUF LETTERS FROM THE PIPE
            perror("Monitor: data read error \n");
        }   
    }
   
    DIR *dp;
    struct dirent *dir;
  
    char* sub_str;
    char* countries_str=malloc(strlen(prev)+1);//STRING WITH ALL THE COUNTRIES
    strcpy(countries_str,prev);
    
    while(sub_str=strtok_r(prev," ",&prev)){//FOR EVERY COUNTRY

        char country_files_dir[11+strlen(sub_str)];
        sprintf(country_files_dir, "%s/%s",input,sub_str);
       
        if((dp=opendir(country_files_dir))==NULL){ // OPEN COUNTRY FILE DIRECTORY 
            perror("Opendir:");
        }

       
        while((dir=readdir(dp))!=NULL){ //FOR EVERY .TXT FILE IN CURRENT COUNTRY
         	
            if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0){ //SKIP . ..
            	continue;
	        }

		
            push_country_files(&first_file,&last_file,dir->d_name);

            char final_name[strlen(country_files_dir)+strlen(dir->d_name)+2];
            sprintf(final_name, "%s/%s",country_files_dir,dir->d_name); //FINAL PATH

            FILE* fd;
		
            if((fd=fopen(final_name, "r+"))==NULL){// OPEN THE CURRENT .TXT
                perror("fopen: ");
            } 
         
            int N = 200; //LINE SIZE
            char line[N] ;	//LINE
         
         
            while(fgets(line, N, fd)){ //GET EVERY LINE
            
                char* citizenID = strtok(line, " "); //ID NUM
                char* firstName = strtok(NULL, " "); //FIRST NAME
                char* lastName = strtok(NULL, " "); //LAST NAME
                char* country = strtok(NULL, " "); //COUNTRY
                char* age = strtok(NULL, " ");//AGE NUM
                char* virusName = strtok(NULL, " "); //VIRUN NAME
                char* YES_OR_NO = strtok(NULL, " "); //YES OR NO
                char* dateVaccinated = " "; 
                
               
                if(strcmp(YES_OR_NO,"YES") == 0 ){
                    dateVaccinated = strtok(NULL, " "); //VACCINATED DATE
                }
            
           
                if(strcmp(YES_OR_NO,"NO") == 0 ){//IF AFTER NO DOESNT CHANGE LINE

                    printf("ERROR IN RECORD %s %s %s %s %s %s %s %s\n",citizenID,firstName,lastName,country,age,virusName,YES_OR_NO,strtok(NULL, " "));
                    continue;
                }
            
                if(Search_for_duplicates_data( first_data, citizenID,  virusName, firstName, lastName, country) == 1){//IF WE HAVE DUPLICATES (SAME ID AND VIRUS)

                    printf("ERROR IN RECORD %s %s %s %s %s %s %s %s\n",citizenID,firstName,lastName,country,age,virusName,YES_OR_NO,dateVaccinated);
                    continue;
                }
            
         
                data_node* temp= push_data(&first_data,&last_data , citizenID, firstName, 
                lastName, country, age, virusName, YES_OR_NO, dateVaccinated );//INSERT NEW DATA
                
                
                if(Search_for_virus_bloom(first_bloom,virusName) == 0) //IF THE BLOOM FILTER FOR THE CURRENT VIRUS DOES NΟT EXIST
                push(&first_bloom,&last_bloom,virusName,bloomSize);//INSERT BLOOM FILTER
                

                if(strcmp(YES_OR_NO,"YES") == 0){ //IF THE CURRENT PERSON HAS BEEN VACCINATED

                    int hash_array[K]; 
                        
                    for (int i = 0 ; i < K ; i++) //FIND THE HASH NUMBERS
                        hash_array[i]=hash_i((unsigned char*)citizenID, i)%bloomSize;
                    
                    Change_bloom_filter(first_bloom,virusName,hash_array,K); //CHANGE THE BLOOM FILTER
                }

            
            }

            fclose(fd);
        }
      
        closedir(dp);
    }
 
    bloom_node* current=first_bloom; 
    while(current!=NULL){// FOR EVERY VIRUS AND BLOOM FILTER
        char* msg=malloc(strlen(current->v_name)+strlen(current->bloom)+5); //CREATE STRING WITH VIRUS AND BLOOM FILTER
        strcpy(msg,current->v_name);
        strcat(msg," ");
        strcat(msg,current->bloom);
        strcat(msg," ");

        int i=0;
        while(i<strlen(msg)){//WRITE VIRUS AND BLOOM FILTERS
           
            strncpy(buf,&msg[i],MAXBUF); 
            i=i+MAXBUF;
            
            if (write(wr, buf,MAXBUF) != MAXBUF)  {
                perror("Travel Monitor: errmesg write error");
            } 
        }

        free(msg);
        current=current->next;
    }
    
    if (write(wr, "#", 1) != 1){//AT THE END WRITE #
        perror("Travel Monitor: errmesg write error");
    }
    


    while(1){

        char* request;
         
        if(sigint_flag==1){//FOR SIGINT SIGQUIT
           sigint_flag=0;
           logfile(accepted,rejected,countries_str);//CREATE LOG FILE
           continue;
        }
        
        if(sigusr1_flag!=1){
       
            if((x=read(re, buf, MAXBUF))<0){ //READ REQUESTS
                if(errno == EINTR){
                    continue;
                }
                perror("Monitor: data read error");
            }
            
            
            prev=malloc(2);
            strcpy(prev," ");
            while ( buf[0]!='#'){ // READ THE PIPE UNTILE YOU GET #
                
                buf[x]='\0';

                char* next=malloc(strlen(prev)+strlen(buf)+1); 
                strcpy(next,prev);
                strcat(next,buf);
                free(prev);

                prev=malloc(strlen(next)+1);
                strcpy(prev,next);
                free(next);

                if ((x = read(re, buf, MAXBUF)) <0) {  //READ MAXBUF LETTERS FROM THE PIPE
                    perror("Monitor: data read error \n");
                }

            }
            
            request = strtok(prev, " ");
        }else{ //FOR SIGUSR1 ACTION

            request= malloc(30);
            strcpy(request,"/addVaccinationRecords");
            sigusr1_flag=0;
        }
       
        if(strcmp(request,"/travelRequest")==0){//-------------------------- /travelRequest ------------------------------
         
            char* ID = strtok(NULL, " ");
            char* date = strtok(NULL, " ");
            char* countryFrom = strtok(NULL, " ");
            char* countryTo = strtok(NULL, " ");
            char* virusName = strtok(NULL, "\n");
            char* ans;
            travelRequest(first_data,ID,virusName,&ans,&accepted,&rejected);
     
            int j=0;
            while(j<strlen(ans)){//WRITE ANSWER

                strncpy(buf,&ans[j],MAXBUF);  
                j=j+MAXBUF;
            
                if (write(wr, buf,MAXBUF) != MAXBUF)  { // WRITE MAXBUF LETTERS EVERY TIME
                perror("Travel Monitor: errmesg write error");
                }
            }

            if (write(wr, "#", 1) != 1)  {//AT THE END WRITE #
                perror("Travel Monitor: errmesg write error");
            }


            free(prev);

        }else if(strcmp(request,"/addVaccinationRecords")==0){//----------------------------- /addVaccinationRecords -----------------------------------
        
            char*str;
            char* cur= malloc(strlen(countries_str)+1);
            strcpy(cur,countries_str);
     
            while(str=strtok_r(cur," ",&cur)){//FOR EVERY COUNTRY
          
         
         
                char country_files_dir[11+strlen(str)];
                sprintf(country_files_dir, "%s/%s",input,str);
            
                if((dp=opendir(country_files_dir))==NULL){ // OPEN COUNTRY FILE DIRECTORY
                    perror("Opendir:");
                }
            
                int i=1;
                while((dir=readdir(dp))!=NULL){ //FOR EVERY .TXT FILE IN THE CURRENT COUNTRY
            
                    if(strcmp(dir->d_name,".")==0||strcmp(dir->d_name,"..")==0){ //SKIP . ..
            		continue;
	    	    }
            
                    if(push_country_files(&first_file,&last_file,dir->d_name)==-1){// IF THE CURRENT COUNTRY FILE ALREADY EXIST CONTINUE
                        continue;
                    }
                
            
                    char final_name[strlen(country_files_dir)+strlen(dir->d_name)+2];
                    sprintf(final_name, "%s/%s",country_files_dir,dir->d_name); //FINAL PATH
            
                    FILE* fd;
                    if((fd=fopen(final_name, "r+"))==NULL){// OPEN THE CURRENT .TXT
                        perror("fopen: ");
                    } 
           
                    int N = 200; //LINE SIZE
                    char line[N] ;	//LINE
            
            
                    while(fgets(line, N, fd)){ //GET EVERY LINE
               
                        char* citizenID = strtok(line, " "); //ID NUM
                        char* firstName = strtok(NULL, " "); //FIRST NAME
                        char* lastName = strtok(NULL, " "); //LAST NAME
                        char* country = strtok(NULL, " "); //COUNTRY
                        char* age = strtok(NULL, " ");//AGE NUM
                        char* virusName = strtok(NULL, " "); //VIRUN NAME
                        char* YES_OR_NO = strtok(NULL, " "); //YES OR NO
                        char* dateVaccinated = " "; 
                        
                        
                        if(strcmp(YES_OR_NO,"YES") == 0 ){
                            dateVaccinated = strtok(NULL, " "); //VACCINATED DATE
                        }
                    
                
                        if(strcmp(YES_OR_NO,"NO") == 0 ){//IF AFTER NO DOESNT CHANGE LINE
                            printf("ERROR IN RECORD %s %s %s %s %s %s %s %s\n",citizenID,firstName,lastName,country,age,virusName,YES_OR_NO,strtok(NULL, " "));
                            continue;
                        }
                
                        if(Search_for_duplicates_data( first_data, citizenID,  virusName, firstName, lastName, country) == 1){//IF WE HAVE DUPLICATES (SAME ID AND VIRUS)
                            printf("ERROR IN RECORD %s %s %s %s %s %s %s %s\n",citizenID,firstName,lastName,country,age,virusName,YES_OR_NO,dateVaccinated);
                            continue;
                        }
                
                
                        data_node* temp= push_data(&first_data,&last_data , citizenID, firstName, 
                        lastName, country, age, virusName, YES_OR_NO, dateVaccinated );//INSERT NEW DATA
                
                
                        if(Search_for_virus_bloom(first_bloom,virusName) == 0) //IF THE BLOOM FILTER FOR THE CURRENT VIRUS DOES NΟT EXIST
                            push(&first_bloom,&last_bloom,virusName,bloomSize);//INSERT BLOOM FILTER
                    

                        if(strcmp(YES_OR_NO,"YES") == 0){ //IF THE CURRENT PERSON HAS BEEN VACCINATED

                            int hash_array[K]; 
                        
                            for (int i = 0 ; i < K ; i++) //FIND THE HASH NUMBERS
                                hash_array[i]=hash_i((unsigned char*)citizenID, i)%bloomSize;
                        
                            Change_bloom_filter(first_bloom,virusName,hash_array,K); //CHANGE THE BLOOM FILTER
                        }

                    }
                    fclose(fd);
                }
     
                closedir(dp);
            }

            bloom_node* current=first_bloom; 
            while(current!=NULL){//FOR EVERY VIRUS AND BLOOM FILTER

                char* msg=malloc(strlen(current->v_name)+strlen(current->bloom)+3);
                strcpy(msg,current->v_name);
                strcat(msg," ");
                strcat(msg,current->bloom);
                strcat(msg," ");
                
                int i=0;
                while(i<strlen(msg)){//WRITE VIRUS AND BLOOM FILTER
               
                    strncpy(buf,&msg[i],MAXBUF); 
                    i=i+MAXBUF;
               
                    if (write(wr, buf,MAXBUF) != MAXBUF)  {
                        perror("Monitor: errmesg write error");
                    }
                }

                free(msg);
                current=current->next;
            }

            if (write(wr, "#", 1) != 1){// AT THE END WRITE #
                perror("Travel Monitor: errmesg write error");
            }
         
        
        }else if(strcmp(request,"/searchVaccinationStatus")==0){// ---------------------------- /searchVaccinationStatus ------------------------

            char* ID = strtok(NULL, "\n");
            char* output;

            if(SearchVaccinationStatus(first_data,ID,&output)!=-1){//IF THE ID EXIST

                int j=0;
                while(j<strlen(output)){//WRITE STATUS

                    strncpy(buf,&output[j],MAXBUF);  
                    j=j+MAXBUF;
            
                    if (write(wr, buf,MAXBUF) != MAXBUF)  { // WRITE MAXBUF LETTERS EVERY TIME
                        perror("Travel Monitor: errmesg write error");
                    }
                }

                if (write(wr, "#", 1) != 1)  {//AT THE END WRITE #
                    perror("Travel Monitor: errmesg write error");
                }

            }else{// IF THE ID DOESNT EXIST

                if (write(wr, "#", 1) != 1)  {//AT THE END WRITE #
                    perror("Travel Monitor: errmesg write error");
                }
            }
        
            free(prev);
        }
  
    }

    close(re);
    close(wr);

    if ( unlink(argv[1]) < 0) { //UNLINK PIPES
        perror("Monitor: can't unlink \n");
    }
    if ( unlink(argv[2]) < 0) {
        perror("Monitor: can't unlink \n");
    }
  
    exit(0);
}

     
