#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"bloom.h"

int push(bloom_node** first,bloom_node** last ,char* virus, int bloomSize ) {  //CREATE NEW BLOOM FILTER

    if(strcmp((*first)->v_name,"-1")==0){ //IF ITS THE FIRST VIRUS
       
        (*first)->v_name=malloc(strlen(virus)+1);
        strcpy((*first)->v_name,virus);

        (*first)->bloom=malloc(bloomSize*sizeof(short));

        for(int i=0; i<bloomSize;i++)
            (*first)->bloom[i]=0;

        return 1;
        
    }
   
    bloom_node* new = malloc(sizeof(bloom_node)); //CREATE NEW NODE

    new->v_name=malloc(strlen(virus)+1);
    strcpy(new->v_name,virus);

    new->bloom=malloc(bloomSize*sizeof(short));
    for(int i=0; i<bloomSize;i++)
            new->bloom[i]=0;
   
    new->next=NULL;
  
    (*last)->next=new;
    (*last)=new;
    return 1;   
} 

int Search_for_virus_bloom( bloom_node* first, char* virus){  //SEARCH IF THE BLOOM FILTER FOR THE CURRENT VIRUS DOES ΝΟΤ EXIST

    bloom_node* current=first;
    while(current->next!=NULL){
        
        if(strcmp(virus,current->v_name)==0) //RETURN 1:THE BLOOM FILTER EXIST
            return 1;
        current = current->next;
    }

    if(strcmp(virus,current->v_name)==0) 
        return 1;

    return 0; //RETURN 0:THE BLOOM FILTER DOES NOT EXIST
}

int Change_bloom_filter(bloom_node* first, char* virus, int hash_array[], int num_of_hash){ //CHANGE BLOOM FILTER

    bloom_node* current=first;
    while(current->next!=NULL){ //FIND THE BLOOM FILTER 
        
        if(strcmp(virus,current->v_name)==0){

            for (int i = 0 ; i < num_of_hash ; i++) //CHANGE THE BITS
                current->bloom[hash_array[i]]=1;

            return 1;
        }
            
        current = current->next;
    }

    if(strcmp(virus,current->v_name)==0){

        for (int i = 0 ; i < num_of_hash ; i++) 
            current->bloom[hash_array[i]]=1;

        return 1;
    }

    return 0;


}

int check_bloom(bloom_node* first, char* virus, int hash_array[],int num_of_hash){//CHECK BLOOM FILTER

    bloom_node* current=first;
    while(current!=NULL){
        
        if(strcmp(virus,current->v_name)==0){ //IF THE BLOOM WITH VIRUS EXIST 
        
            for (int i = 0 ; i < num_of_hash ; i++) 
                if(current->bloom[hash_array[i]]==0){
                    printf("\nNOT VACCINATED\n");
                    return 0;//NOT
                }
            printf("\nMAYBE\n");    
            return 1;//MAYBE
        }
        current = current->next;
    }
    printf("\nWRONG INPUT\n");
    return -1; //WRONG VIRUS




}