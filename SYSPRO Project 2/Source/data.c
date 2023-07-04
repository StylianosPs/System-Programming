#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "data.h"

data_node* push_data(data_node** first,data_node** last ,char* citizenID, char* firstName, 
char* lastName, char* country, char* age, char* virusName, char* YES_OR_NO, char* dateVaccinated ) {  //INSERT NEW DATA NODE

   
    if(strcmp((*first)->citizenID,"-1")==0){ //IF ITS THE FIRST PERSON
      
        (*first)->citizenID=malloc(strlen(citizenID)+1);
        (*first)->country=malloc(strlen(country)+1);
        (*first)->age=malloc(strlen(age)+1);
        (*first)->firstName=malloc(strlen(firstName)+1);
        (*first)->lastName=malloc(strlen(lastName)+1);
      
        strcpy((*first)->citizenID,citizenID);
        strcpy((*first)->country,country);
        strcpy((*first)->age,age);
        strcpy((*first)->firstName,firstName);
        strcpy((*first)->lastName,lastName);
       
        virus* new= malloc(sizeof(virus));
        new->virusName=malloc(strlen(lastName)+1);
        strcpy(new->virusName,virusName);
        new->date=malloc(strlen(dateVaccinated)+1);
        new->y_n=malloc(strlen(YES_OR_NO)+1);
     
        strcpy(new->y_n,YES_OR_NO);
        strcpy(new->date,dateVaccinated);
        new->next=NULL;
        (*first)->v_name=new;

        return (*first);
        
    }
    

    data_node* current=(*first);
    while(current!=NULL){
        if(strcmp(current->citizenID,citizenID)==0){//IF WE ALREADY HAVE THE CITIZENID
            virus* v=current->v_name;
            while(v->next!=NULL){

                if(strcmp(v->virusName,virusName)==0){//IF THE VIRUS ALREADY EXIST
                    if(strcmp(v->y_n,"NO")==0||strcmp(v->y_n,"NO\n")==0){
                        free(v->y_n);
                        v->y_n=malloc(strlen(YES_OR_NO)+1);
                        strcpy(v->y_n,YES_OR_NO);
                        free(v->date);
                        v->date=malloc(strlen(dateVaccinated)+1);
                        strcpy(v->date,dateVaccinated);
                    }
                    return current;
                }
                v=v->next;
            }

            if(strcmp(v->virusName,virusName)==0){//IF THE VIRUS ALREADY EXIST
                    if(strcmp(v->y_n,"NO")==0||strcmp(v->y_n,"NO\n")==0){
                        free(v->y_n);
                        v->y_n=malloc(strlen(YES_OR_NO)+1);
                        strcpy(v->y_n,YES_OR_NO);
                        free(v->date);
                        v->date=malloc(strlen(dateVaccinated)+1);
                        strcpy(v->date,dateVaccinated);
                    }
                    return current;
            }
            else{//IF THE VIRUS DOESNT EXIST
                virus* new= malloc(sizeof(virus));
                new->virusName=malloc(strlen(virusName)+1);
                strcpy(new->virusName,virusName);
                new->date=malloc(strlen(dateVaccinated)+1);
                new->y_n=malloc(strlen(YES_OR_NO)+1);
                
                strcpy(new->y_n,YES_OR_NO);
                strcpy(new->date,dateVaccinated);
                new->next=NULL;
                v->next=new;
                return current;
            }

        }
        current=current->next;
    }

   
    data_node* new = malloc(sizeof(data_node)); //CREATE NEW NODE

    new->citizenID=malloc(strlen(citizenID)+1);
    new->country=malloc(strlen(country)+1);
    new->age=malloc(strlen(age)+1);
    new->firstName=malloc(strlen(firstName)+1);
    new->lastName=malloc(strlen(lastName)+1);

    strcpy(new->citizenID,citizenID);
    strcpy(new->country,country);
    strcpy(new->age,age);
    strcpy(new->firstName,firstName);
    strcpy(new->lastName,lastName);

    virus* new1= malloc(sizeof(virus));
    new1->virusName=malloc(strlen(virusName)+1);
    strcpy(new1->virusName,virusName);
    new1->date=malloc(strlen(dateVaccinated)+1);
    new1->y_n=malloc(strlen(YES_OR_NO)+1);

    strcpy(new1->y_n,YES_OR_NO);
    strcpy(new1->date,dateVaccinated);
    new1->next=NULL;
    new->v_name=new1;
   
    new->next=NULL;
  
    (*last)->next=new;
    (*last)=new;
    return (*last);   
}

int Search_for_duplicates_data( data_node* first,char* id,  char* virus_n, char* f_name, char* l_name, char* country){  //SEARCH THE DATA FOR DUPLICATES OR ERRORS
    
    data_node* current=first;
    while(current!=NULL){
        
        if(strcmp(id,current->citizenID) == 0){ //IF THE CITIZEND EXIST
            
            if(strcmp(f_name,current->firstName)!=0 || strcmp(l_name,current->lastName)!=0 || strcmp(country,current->country)!=0){
                return 1;
            }
            virus* v=current->v_name;
            while(v!=NULL){

                
                if(strcmp(v->virusName,virus_n)==0)//IF THE VIRUS ALREADY EXIST   
                    return 1;
                
                v=v->next;
            }
        
        }  
        current = current->next;
    }


    return 0; //RETURN 0:THE DATA DOES NOT EXIST
}

int travelRequest(data_node* first, char* id, char* v_name,char** ans, int* accepted, int* rejected ) {

    data_node* current=first;
  
    while(current!=NULL){//FOR EVERY CITIZEN

        if(strcmp(current->citizenID,id)==0){//IF THE ID EXIST
           
            virus* current1=current->v_name;
            while(current1!=NULL){//FOR EVERY VIRUS 

                if(strcmp(current1->virusName,v_name)==0){//IF THE VIRUS EXIST

                    if(strcmp(current1->y_n,"YES")==0){//IF THE CITIZEND IS VACCINATED
                        (*accepted)++;
                        (*ans)=malloc(6+strlen( current1->date));
                        sprintf((*ans), "YES %s",current1->date);
                        
                        return 1;

                    }else{//IF THE CITIZEN IS NOT VACCINATED
                        (*rejected)++;
                        (*ans)=malloc(4);
                        strcpy( (*ans),"NO");
                        return 1;
                    }

                }
                
                current1=current1->next;
            }

        }


        current=current->next;
    }

    (*ans)=malloc(4);
    strcpy( (*ans),"NO");
    return 1;
}


int push_country_files(country_files** first, country_files** last, char* file) {  
    
    country_files* current=(*first);
    while(current!=NULL){
        if(strcmp(current->file,file)==0){//IF THE FILE ALREADY EXIST
            return -1;
        }

        current=current->next;
    }

    if(strcmp((*first)->file,"-1")==0){//IF ITS THE FIRST FILE 
      
        (*first)->file=malloc(strlen(file)+1);
        strcpy((*first)->file,file);
    
        return 1;  
    }
   
    country_files* new = malloc(sizeof(country_files)); //CREATE NEW NODE 
    new->file=malloc(strlen(file)+1);  
    strcpy(new->file,file);
    new->next=NULL;
  
    (*last)->next=new;
    (*last)=new;
    return 1;   
} 

int SearchVaccinationStatus(data_node* first, char* ID, char** output ){

    data_node* current=first;
    while(current!=NULL){
        if(strcmp(current->citizenID,ID)==0){
           (*output)=malloc(strlen(current->citizenID)+strlen(current->firstName)+strlen(current->lastName)+strlen(current->country)+strlen(current->age)+11);
           sprintf((*output),"%s %s %s %s\nAGE %s\n",current->citizenID, current->firstName, current->lastName, current->country, current->age);
           virus* current2=current->v_name;
            while(current2!=NULL){
                if(strcmp(current2->y_n,"YES")==0){
                    char* next=malloc(strlen((*output))+strlen(current2->date)+strlen(current2->virusName)+20);
                    sprintf(next,"%s%s VACCINATED ON %s",(*output),current2->virusName,current2->date);
                    free((*output));

                    (*output)=malloc(strlen(next)+1);
                    strcpy((*output),next);
                    free(next);
                }else{
                    char* next=malloc(strlen((*output))+strlen(current2->virusName)+25);
                    sprintf(next,"%s%s NOT YET VACCINATED",(*output),current2->virusName);
                    free((*output));

                    (*output)=malloc(strlen(next)+1);
                    strcpy((*output),next);
                    free(next);
                }
               current2=current2->next;
            }
           return 1;
        }

        current=current->next;
    }
    return -1;

}


int logfile(int accepted, int rejected, char* countries){

    char filename[30];
    sprintf(filename,"log_file.%d.txt",getpid());
    FILE* fd=fopen(filename,"w");//CREATE LOGFILE

    char* sub_str;
    while(sub_str=strtok_r(countries," ",&countries)){//FOR EVERY COUNTRY

        fputs(sub_str,fd);//WRITE COUNTRY
        fputs("\n",fd);

    }

    char output[100];
    sprintf(output,"TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n",(accepted+rejected),accepted,rejected);
    fputs(output,fd);
    fputs("\n",fd);
    
    fclose(fd);
    return 1;
}