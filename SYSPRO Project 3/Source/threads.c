#include <stdio.h>
#include <string.h>	
#include <stdlib.h>
#include "threads.h"

int initialize_file_buffer(file_buffer* node, int cyclicbuffersize){//INITIALIZE CYCLIC BUFFER

    node->file=malloc(cyclicbuffersize *sizeof(char*));
    node->first = 0;
    node->last = -1;
    node->counter = 0;
    node->size = cyclicbuffersize;


    return 1;
}

int add_file(file_buffer* node, char* file){//ADD PATH IN CYCLIC BUFFER
    node->counter++;
    node->last = (node->last + 1) % node->size;
    node->file[node->last] = malloc(strlen(file)+1);
    strcpy(node->file[node->last], file);

    return 1;
}

int put_records_in_structurs(data_node** first_data, data_node** last_data, bloom_node** first_bloom, bloom_node** last_bloom, 
                            char* final_name, int bloomSize ){

    FILE* fd;
		
    if((fd=fopen(final_name, "r+"))==NULL){// OPEN THE CURRENT .TXT
        perror("fopen: ");
    } 
    
    int N = 200; //LINE SIZE
    char line[N] ;	//LINE
    int K =16;
    
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
    
        data_node* temp= push_data(first_data,last_data , citizenID, firstName, 
        lastName, country, age, virusName, YES_OR_NO, dateVaccinated );//INSERT NEW DATA
        
        
        if(Search_for_virus_bloom(*first_bloom,virusName) == 0) //IF THE BLOOM FILTER FOR THE CURRENT VIRUS DOES NΟT EXIST
        push(first_bloom,last_bloom,virusName,bloomSize);//INSERT BLOOM FILTER
        

        if(strcmp(YES_OR_NO,"YES") == 0){ //IF THE CURRENT PERSON HAS BEEN VACCINATED

            int hash_array[K]; 
                
            for (int i = 0 ; i < K ; i++) //FIND THE HASH NUMBERS
                hash_array[i]=hash_i((unsigned char*)citizenID, i)%bloomSize;
            
            Change_bloom_filter(*first_bloom,virusName,hash_array,K); //CHANGE THE BLOOM FILTER
        }

    
    }

    fclose(fd);

    return 1;


}
