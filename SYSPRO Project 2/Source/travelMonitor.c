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
#include "tmonitor_bloom.h"
#include "bloom.h"

extern int errno;

#define PERMS   0666
int sigint_flag=0;
int sigchld_flag=0;

void handler_sigint(int sigo){
   sigint_flag=1;
}

void handler_sigchld(int sigo){

    sigchld_flag=1;
    
}

int q_sort_compare(const void *x, const void *y){
    return( strcmp(*(const char **)x,*(const char **)y) );
}
int main(int argc, char** argv){
   
    if(argc!=9){
        printf("WRONG ARGUMENTS\n");
        return -1;
    }

    int num_monitor =atoi(argv[2]);
    int MAXBUF= atoi(argv[4]);
    int re[num_monitor], wr[num_monitor];
    char buf[MAXBUF];
    int pid_array[num_monitor];
    int bloomSize =atoi(argv[6]);
    int K=16;

    if(num_monitor<1 || MAXBUF<1 || bloomSize<1){
        printf("WRONG ARGUMENTS\n");
        return -1;
    }

    struct sigaction sa = {0};
    sa.sa_handler = &handler_sigint;   
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);

    struct sigaction sa2 = {0};
    sa2.sa_handler = &handler_sigchld;   
    sigaction(SIGCHLD, &sa2, NULL);

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

    for(int i=1; i<=num_monitor; i++){ //CREATE FIFOS AND MONITOR

        char fifo_w[60];
        char fifo_r[60];
        sprintf(fifo_w, "/tmp/fifo.%d",((2*i)-1));
        sprintf(fifo_r, "/tmp/fifo.%d",(2*i));
      
        if ( (mkfifo(fifo_r, PERMS) < 0) && (errno != EEXIST) ) { //CREATE READ FIFO
            perror("can't create fifo");
        }
        if ((mkfifo(fifo_w, PERMS) < 0) && (errno != EEXIST)) { //CREATE WRITE FIFO
            unlink(fifo_r);
            perror("can't create fifo");
        }

        pid_array[i-1]=fork(); //CREATE MONITORS
            
        if(pid_array[i-1]==-1){ 
            perror("Failed to fork");
            return 1;
        }
        
        if(pid_array[i-1]==0){// CHILD: EXEC
            
            execlp("./monitor","monitor",fifo_r,fifo_w,NULL);

            perror("execlp");
        }


        if ( (re[i-1] = open(fifo_r, 0))  < 0)  { //OPEN READ FIFO
            perror("Travel Monitor: can't open read fifo");
        }
    
        if ( (wr[i-1] = open(fifo_w, 1))  < 0)  { //OPEN WRITE FIFO
            perror("Travel Monitor: can't open write fifo");
        }
    }

   
    DIR *dp;
    struct dirent *dir;
    int j=0;

    if((dp=opendir(argv[8]))==NULL){ // OPEN input_dir
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
	

        dir_array = realloc(dir_array, (dir_counter+1)*sizeof(*dir_array));
        dir_array[dir_counter] = malloc((strlen(dir->d_name) +1)* sizeof(char*));
        strcpy(dir_array[dir_counter], dir->d_name); 
        dir_counter++;

    }
   
    

    for(int j=0; j<num_monitor;j++){ //WRITE IN PIPE MAXBUFFER BLOOMSIZE AND DIRECTORY

        if (write(wr[j], argv[4],10) != 10)  { //MAXBUFFER
            perror("Travel Monitor: errmesg write error");
        }
        
        if (write(wr[j], argv[6],10) != 10)  { //BLOOMSIZE
            perror("Travel Monitor: errmesg write error");
        }

        if (write(wr[j], argv[8],30) != 30)  { //DIRECTORY
            perror("Travel Monitor: errmesg write error");
        }
    }

    qsort(dir_array,dir_counter,sizeof(char *),q_sort_compare);

    for(int y=0; y<dir_counter; y++){ //READ COUNTRIES FROM DIRECTORY

        int i=0;
        push_country(&first_monitor_bloom,&last_monitors_bloom,j,dir_array[y]); //PUSH NODE IN MONITORS_BLOOM FOR EVERY COUNTRY
      
        char* co=malloc(strlen(dir_array[y])+3); //COUNTRY
        strcpy(co,dir_array[y]);
        strcat(co," ");
        while(i<strlen(co)){ //WRITE COUNTRY 
            
            strncpy(buf,&co[i],MAXBUF);  
            i=i+MAXBUF;
            
            if (write(wr[j], buf,MAXBUF) != MAXBUF){ // WRITE MAXBUF LETTERS EVERY TIME
                perror("Travel Monitor: errmesg write error");
            }
        }
        
        j++;
        
        if(j==num_monitor){ //ROUND ROBIN: IF ITS THE LAST MONITOR GO TO THE FIRST AGAIN
            j=0;
        }

        free(co);
    }
  

    for(int j=0; j<num_monitor;j++) // AT THE END WRITE # 
        if (write(wr[j], "#", 1) != 1){
            perror("Travel Monitor: errmesg write error");
        }
    
    for(int y=0; y<dir_counter; y++)
        free(dir_array[y]);
    
    free(dir_array);

   
    char b[MAXBUF+2];
    int x;

    for(int j=0; j<num_monitor;j++){ //GET VIRUS AND BLOOM FILTERS FOR EVERY MONITOR
      
        if((x=read(re[j], buf, MAXBUF))<0){ //READ THE FIRST MAXBUF LETTERS
            perror("Monitor: data read error");
        }

        char* prev=malloc(2);
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

            if ((x = read(re[j], buf, MAXBUF)) <0) {  //READ MAXBUF LETTERS FROM THE PIPE
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
        
        if(sigchld_flag==1){  //IF WE HAVE SIGCHLD ACTION  
            
            sigchld_flag==0;
            for(int i=0; i<num_monitor; i++){//FIND THE DEAD CHILD
                int status;
                int child = waitpid(pid_array[i],&status,WNOHANG);
               
                if(child > 0){

                    char fifo_w_new[60];
                    char fifo_r_new[60];
                    sprintf(fifo_w_new, "/tmp/fifo.%d",((2*(i+1))-1));
                    sprintf(fifo_r_new, "/tmp/fifo.%d",(2*(i+1)));
      
                    if ( (mkfifo(fifo_r_new, PERMS) < 0) && (errno != EEXIST) ) { //CREATE READ FIFO
                        perror("can't create fifo");
                    }
                    if ((mkfifo(fifo_w_new, PERMS) < 0) && (errno != EEXIST)) { //CREATE WRITE FIFO
                        unlink(fifo_r_new);
                        perror("can't create fifo");
                    }

                    pid_array[i]=fork(); //CREATE MONITORS
            
                    if(pid_array[i]==-1){ 
                        perror("Failed to fork");
                        return 1;
                    }
                    
                    if(pid_array[i]==0){// CHILD: EXEC
            
                        execlp("./monitor","monitor",fifo_r_new,fifo_w_new,NULL);

                        perror("execlp");
                    }


                    if ( (re[i] = open(fifo_r_new, 0))  < 0)  { //OPEN READ FIFO
                        perror("Travel Monitor: can't open read fifo");
                    }
    
                    if ( (wr[i] = open(fifo_w_new, 1))  < 0)  { //OPEN WRITE FIFO
                        perror("Travel Monitor: can't open write fifo");
                    }

                    
                    if (write(wr[i], argv[4],10) != 10)  { //MAXBUFFER
                        perror("Travel Monitor: errmesg write error");
                    }
                    
                    if (write(wr[i], argv[6],10) != 10)  { //BLOOMSIZE
                        perror("Travel Monitor: errmesg write error");
                    }

                    if (write(wr[i], argv[8],30) != 30)  { //DIRECTORY
                        perror("Travel Monitor: errmesg write error");
                    }

                    monitors_bloom* current=first_monitor_bloom;
                    while(current!=NULL){
                        
                        if(current->monitor==i){
                            countries* current2=current->f_country;
                            while(current2!=NULL){

                                int j=0;
      
                                char* co=malloc(strlen(current2->country)+3); //COUNTRY
                                strcpy(co,current2->country);
                                strcat(co," ");

                                while(j<strlen(co)){ //WRITE COUNTRY 
            
                                    strncpy(buf,&co[j],MAXBUF);  
                                    j=j+MAXBUF;
            
                                    if (write(wr[i], buf,MAXBUF) != MAXBUF){ // WRITE MAXBUF LETTERS EVERY TIME
                                        perror("Travel Monitor: errmesg write error");
                                    }
                                }
    
                                free(co);

                                current2=current2->next;
                            }
                            if (write(wr[i], "#", 1) != 1){
                                perror("Travel Monitor: errmesg write error");
                            }
                            break;
                        }

                        current=current->next;
                    }
                 
                    if((x=read(re[i], buf, MAXBUF))<0){ //READ THE FIRST MAXBUF LETTERS
                        perror("Monitor: data read error");
                    }

                    char* prev=malloc(2);
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

                        if ((x = read(re[i], buf, MAXBUF)) <0) {  //READ MAXBUF LETTERS FROM THE PIPE
                            perror("Monitor: data read error \n");
                        }
                        
            
            
                    }

                    free(prev);
                    
                }
                ///////////////

            }
        }

        if(sigint_flag!=1){ 

            char cm[200];
            printf("\nGIVE INPUT: ");
            fgets(cm, 200, stdin);

            cm2=malloc(strlen(cm)+1);
            strcpy(cm2,cm);
            
            request = strtok(cm, " ");

        }else{ // IF WE HAVE SIGINT OR SIGQUIT
        
            request=malloc(10);
            cm2=malloc(2);
            strcpy(request,"/exit\n");
            sigint_flag=0;

        }
      

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
                 
                    while(j<strlen(cm2)){//WRITE THE REQUEST IN PIPE
         
                        strncpy(buf,&cm2[j],MAXBUF);  
                        j=j+MAXBUF;
                     
                        if (write(wr[p], buf,MAXBUF) != MAXBUF)  { // WRITE MAXBUF LETTERS EVERY TIME
                            perror("Travel Monitor: errmesg write error");
                        }
                    }

                    if (write(wr[p], "#", 1) != 1)  { //AT THE END WRITE #
                        perror("Travel Monitor: errmesg write error");
                    }
			
                
                    if((x=read(re[p], buf, MAXBUF))<0){ //READ THE ANSWER
                        perror("Travel Monitor: data read error");
                    }
      
                    char* prev=malloc(2);
                    strcpy(prev," ");

                    while ( buf[0]!='#'){ // READ THE PIPE UNTILE YOU GET #
                     
                        buf[x]='\0';

                        char* next=malloc(strlen(prev)+strlen(buf)+3); 
                        strcpy(next,prev);
                        strcat(next,buf);
                        free(prev);
                    
                        prev=malloc(strlen(next)+1);
                        strcpy(prev,next);
                        free(next);

                        if ((x = read(re[p], buf, MAXBUF)) <0) {  //READ MAXBUF LETTERS FROM THE PIPE
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

                kill(pid_array[p],SIGUSR1); // SIGUSR1 ACTION
            
                if((x=read(re[p], buf, MAXBUF))<0){ //READ THE UPDATE BLOOM FILTERS
                    perror("Monitor: data read error");
                }

                char* prev=malloc(2);
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
              
                    if ((x = read(re[p], buf, MAXBUF)) <0) {  //READ MAXBUF LETTERS FROM THE PIPE
                        perror("Monitor: data read error \n");
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
        
            }else{
                printf("WRONG INPUT\n");
            }

        }else if(strcmp(request,"/searchVaccinationStatus")==0){//-------------------------- /searchVaccinationStatus -----------------------------------------
            
            char* ID = strtok(NULL, "\n");   

            if(ID!=NULL){

                for(int i=0; i<num_monitor;i++){ //CHECK EVERY MONITOR

                    int j=0;
                    while(j<strlen(cm2)){
      
                        strncpy(buf,&cm2[j],MAXBUF);  
                        j=j+MAXBUF;
                        
                        if (write(wr[i], buf,MAXBUF) != MAXBUF)  { 
                            perror("Travel Monitor: errmesg write error");
                        }
                    }

                    if (write(wr[i], "#", 1) != 1)  {
                        perror("Travel Monitor: errmesg write error");
                    }
                }

                for(int i=0; i<num_monitor;i++){

                    if((x=read(re[i], buf, MAXBUF))<0){ //READ STATUS
                        perror("Travel Monitor: data read error");
                        exit(0);
                    }
      
                    char* prev=malloc(2);
                    strcpy(prev," ");

                    while ( buf[0]!='#'){ // READ THE PIPE UNTILE YOU GET #
                  
                        buf[x]='\0';

                        char* next=malloc(strlen(prev)+strlen(buf)+3); 
                        strcpy(next,prev);
                        strcat(next,buf);
                        free(prev);
                
                        prev=malloc(strlen(next)+1);
                        strcpy(prev,next);
                        free(next);

                        if ((x = read(re[i], buf, MAXBUF)) <0) {  //READ MAXBUF LETTERS FROM THE PIPE
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
       
            for(int j=0; j<num_monitor;j++) //KILL
                kill(pid_array[j],SIGKILL);

            for(int j=0; j<num_monitor;j++) //WAIT 
                waitpid(pid_array[j],NULL,0);

            for(int j=0; j<num_monitor;j++){ //CLOSE PIPES
                close(re[j]);
                close(wr[j]);
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

            free(cm2);
            break;    

        }else{ 
            printf("WRONG INPUT\n");
        }
        free(cm2);
      
    }

    return 1;
}

     
