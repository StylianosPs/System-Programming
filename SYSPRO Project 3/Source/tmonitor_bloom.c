#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "tmonitor_bloom.h"



int push_country(monitors_bloom** first,monitors_bloom** last ,int mon,char* country) {  //INSERT NEW BLOOM FILTER FOR EVERY COUNTRY

    if((*first)->monitor==-1){ //IF ITS THE FIRST MONITOR
      
        (*first)->monitor=mon;
        
        countries* new= malloc(sizeof(countries));
        new->country=malloc(strlen(country)+1);
        strcpy(new->country,country);
        new->next=NULL;

        (*first)->f_country=new;
        (*first)->f_bloom=NULL;

        return 1; 
    }

    monitors_bloom* current=(*first);
    while(current!=NULL){
        if(current->monitor==mon){ //IF THE MONITOR ALREADY EXIST

            countries* new= malloc(sizeof(countries));
            new->country=malloc(strlen(country)+1);
            strcpy(new->country,country);
            new->next=current->f_country;

            current->f_country=new;
            return 1;
        }
        current=current->next;
    }
    
    countries* new= malloc(sizeof(countries)); //NEW MONITOR
    new->country=malloc(strlen(country)+1);
    strcpy(new->country,country);
    new->next=NULL;

    monitors_bloom* new1 = malloc(sizeof(monitors_bloom));
    new1->monitor=mon;
    new1->f_country=new;
    new1->next=NULL;
    new1->f_bloom=NULL;

    (*last)->next=new1;
    (*last)=new1;

    return 1;

}

int push_bloom(monitors_bloom** first,monitors_bloom** last,int mon ,char* virus ,char* bloom){//INSERT BLOOM AND VIRUS

    monitors_bloom* current=(*first);
    while(current!=NULL){

        if(current->monitor==mon){//INSERT VIRUS AND BLOOM FILTER IN MONITOR_BLOOM NODE
            bloom_filters* new=malloc(sizeof(bloom_filters));
            new->virus = malloc(strlen(virus)+1);
            new->bloom = malloc(strlen(bloom)+1);
            strcpy(new->virus,virus);
            strcpy(new->bloom,bloom);

            new->next=current->f_bloom;
            current->f_bloom=new;
            return 1;
        }

        current=current->next;
    }
}

int find_monitor(monitors_bloom* first, char* country){//FIND MONITOR

    monitors_bloom* current=first;

    while(current!=NULL){
        countries* current1=current->f_country;
        while(current1!=NULL){
           if(strcmp(current1->country,country)==0)
            return current->monitor; 
            
            current1=current1->next;
        }

        current=current->next;
    }

    return -1;
}

int search_blooms(monitors_bloom* first,char* countryfrom, char* virus, int hash_array[],int num_of_hash){

    monitors_bloom* current=first;
    while(current!=NULL){

        countries* current1=current->f_country;
        while(current1!=NULL){
            if(strcmp(current1->country,countryfrom)==0){
                bloom_filters* current2=current->f_bloom;
                while(current2!=NULL){
                    if(strcmp(current2->virus,virus)==0){
                        for (int i = 0 ; i < num_of_hash ; i++){
                            if(current2->bloom[hash_array[i]]=='0')
                                return 0;//NOT
                            
                        }  
                            
                        return 1;//MAYBE         
                    }
                    current2=current2->next;
                }
            }
            current1=current1->next;
        }

        current=current->next;
    }
}

int compare_dates(char* d1,char* d2){
    
    char date[12];
    strcpy(date,d1);
    int day1 =atoi(strtok(date, "-"));
    int month1 = atoi(strtok(NULL, "-"));
    int year1 = atoi(strtok(NULL, " "));
    strcpy(date,d2);
    int day2 =atoi(strtok(date, "-"));
    int month2 = atoi(strtok(NULL, "-"));
    int year2 = atoi(strtok(NULL, " "));
    
    if(year1==year2){
        
        if(month1<(month2+6)&&(month1>month2)){
            return 0;
        }
        if(month1==(month2+6)){
            if(day2>=day1){
                return 0;
            }
        }
       
        return 1;
    }

    if((year1-1)==year2){
        
        if((12-month2+month1)<6){
            return 0;
        }
        if((12-month2+month1)==6){
            if(day2>=day1){
                return 0;
            }
        }
       
        return 1;
    }
  
    return 1;
}

int push_request(request_node** first, request_node** last, char* country, char* date, char* virus, int ac_re) {  
   
    if(strcmp((*first)->country,"-1")==0){ //IF ITS THE FIRST REQUEST
       
        (*first)->country=malloc(strlen(country)+1);
        strcpy((*first)->country,country);

        dates* new=malloc(sizeof(dates));
        new->date=malloc(strlen(date)+1);
        strcpy(new->date,date);
        new->virus=malloc(strlen(virus)+1);
        strcpy(new->virus,virus);

        new->ac_re=ac_re;
        new->next=NULL;

        (*first)->f_date=new;
        return 1;
        
    }

    request_node* current=(*first);
    while(current!=NULL){
        if(strcmp(current->country,country)==0){//IF THE COUNTRY EXIST
            dates* new=malloc(sizeof(dates));
            new->date=malloc(strlen(date)+1);
            strcpy(new->date,date);
            new->virus=malloc(strlen(virus)+1);
            strcpy(new->virus,virus);

            new->ac_re=ac_re;
            new->next=current->f_date;
            current->f_date=new;
            return 1;
        }

        current=current->next;
    }
   
    request_node* new = malloc(sizeof(request_node)); //CREATE NEW NODE

    
    new->country=malloc(strlen(country)+1);  
    strcpy(new->country,country);
    
    dates* new1=malloc(sizeof(dates));
    new1->date=malloc(strlen(date)+1);
    strcpy(new1->date,date);
    new1->virus=malloc(strlen(virus)+1);
    strcpy(new1->virus,virus);

    new1->ac_re=ac_re;
    new1->next=NULL;

    new->f_date=new1;
    new->next=NULL;
  
    (*last)->next=new;
    (*last)=new;
    return 1;   
} 

int compare_dates_for_status(char* d,char* d1,char* d2){
    
    char date[12];
    strcpy(date,d);
    int day =atoi(strtok(date, "-"));
    int month = atoi(strtok(NULL, "-"));
    int year = atoi(strtok(NULL, " "));

    
    strcpy(date,d1);
    int day1 =atoi(strtok(date, "-"));
    int month1 = atoi(strtok(NULL, "-"));
    int year1 = atoi(strtok(NULL, " "));
    strcpy(date,d2);
    int day2 =atoi(strtok(date, "-"));
    int month2 = atoi(strtok(NULL, "-"));
    int year2 = atoi(strtok(NULL, " "));
    
    

    if(year1<year && year<year2)//IF THE DATE IS BETWEEN 
        return 1;

    if(year1==year && year<year2)// IF THE DATE IS IN THE  YEAR1
        if(month1<month)//AFTER THE MONTH1
            return 1;
        else if(month1==month)//IN THE MONTH1
            if(day1<=day)//AFTER THW DAY 1
                return 1;

    if(year1<year && year==year2)//IF THE DATE IS IN THE YEAR2
        if(month<month2)//BEFORE THE  MONTH2
            return 1;
        else if(month==month2)//IN MONTH2
            if(day<=day2)//BEFORE THE DAY2
                return 1;

    
    
    if(year1==year && year==year2)//IF THE ITS THE SAME YEAR    
        if(month1<month && month<month2)//BETWEEN THE MONTHS
            return 1;
        else if(month1==month && month<month2){//IN THE MONTH1
            if(day1<day)//AFTER THE DAY1
                return 1;
        }else if(month1<month && month==month2){//IN THE MONTH 2
            if(day<day2)//BEFORE THE DAY2
                return 1;
        }else if(month1==month && month==month2)//IF ITS THE SAME MONTH
            if(day1<=day && day<=day2)//BETWEEN 
                return 1;
        
    return -1;
}

int travelStatus(request_node* first, char* country,char* virus, char* date1, char* date2){

    int accepted=0, rejected=0;

    if(strcmp(country,"-1")==0){// WITHOUT COUNTRY

        request_node* current=first;
        while(current!=NULL){//FOR EVERY REQUEST (COUNTRY)
            accepted=0;
            rejected=0;

            printf("\nCOUNTRY: %s\n",current->country);

            dates* current2=current->f_date;
            while(current2!=NULL){
                if(strcmp(current2->virus,virus)==0){
                    if(compare_dates_for_status(current2->date,date1,date2)==1){//IF THE REQUEST DATE IS BETWEEN THE TWO DATES
                        if(current2->ac_re==1){
                            accepted++;
                        }else{
                            rejected++;
                        }
                    }
                }
                current2=current2->next;
            }
         
            printf("TOTAL REQUEST: %d\nACCEPTED: %d\nREJECTED:%d\n",(accepted+rejected),accepted,rejected);  
                
                
            current=current->next;
        }
        return 1;
    }

    request_node* current=first;
    while(current!=NULL){//WITH COUNTRY
        if(strcmp(country,current->country)==0){

            printf("\ncountry: %s\n",current->country);

            dates* current2=current->f_date;
            while(current2!=NULL){
                if(strcmp(current2->virus,virus)==0){
                    if(compare_dates_for_status(current2->date,date1,date2)==1){//CHECK IF THE REQUEST DTE IS BETWEEN THE TWO DATES
                        if(current2->ac_re==1){
                            accepted++;
                        }else{
                            rejected++;
                        }
                    }
                }
                current2=current2->next;
            }
            printf("TOTAL REQUEST: %d\nACCEPTED: %d\nREJECTED:%d\n",(accepted+rejected),accepted,rejected);
            return 1;
        }
        
        
        current=current->next;
    }
    return -1;
}

int update_bloom(monitors_bloom** first,monitors_bloom** last,int mon ,char* virus ,char* bloom){

    monitors_bloom* current=(*first);
    while(current!=NULL){

        if(current->monitor==mon){
            bloom_filters* current1=current->f_bloom;
            while(current1!=NULL){//UPDATE: IF THE BLOOM FILTER EXIST
                if(strcmp(current1->virus,virus)==0){
                    free(current1->bloom);
                    current1->bloom = malloc(strlen(bloom)+1);
                    strcpy(current1->bloom,bloom);
                    return 1;
                }
                

                current1=current1->next;
            }
            
            bloom_filters* new=malloc(sizeof(bloom_filters));//UPDATE IF THE BLOOM FILTER DOESNT EXIST
            new->virus = malloc(strlen(virus)+1);
            new->bloom = malloc(strlen(bloom)+1);
            strcpy(new->virus,virus);
            strcpy(new->bloom,bloom);
           

            new->next=current->f_bloom;
            current->f_bloom=new;
            return 1;
        }

        current=current->next;
    }
}

int logfile(request_node* first_requesr, monitors_bloom* first_bloom){
    char filename[30];
    int accepted=0,rejected=0;
    sprintf(filename,"log_file.%d.txt",getpid());
    FILE* fd=fopen(filename,"w");//CREATE AND OPEN LOG FILE

    monitors_bloom* current=first_bloom;
    while(current!=NULL){//COUNTRIES

        countries* current1=current->f_country;
        while(current1!=NULL){
            fputs(current1->country,fd);
            fputs("\n",fd);
            current1=current1->next;
        }

        current=current->next;
    }

    request_node* current2=first_requesr;
    while(current2!=NULL){//SUM ACCEPTED AND REJECTED REQUEST 
        
        dates* current3=current2->f_date;
        while(current3!=NULL){
                
            if(current3->ac_re==1){
                accepted++;
            }else{
                rejected++;
            }
                
            current3=current3->next;
        }   
        current2=current2->next;
    }

    char output[100];
    sprintf(output,"TOTAL TRAVEL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n",(accepted+rejected),accepted,rejected);
    fputs(output,fd);
    fputs("\n",fd);
    
    fclose(fd);
    return 1;
}