#include <stdlib.h>
#pragma once

typedef struct bloom_node {
    char* v_name;  //VIRUS NAME
    short* bloom; //BLOOM FILTER
    struct bloom_node *next;
    
} bloom_node;



int push(bloom_node** first,bloom_node** last ,char* virus, int bloomSize );  //CREATE NEW BLOOM FILTER
int Search_for_virus_bloom( bloom_node* first, char* virus);  //SEARCH IF THE BLOOM FILTER FOR THE CURRENT VIRUS DOES ΝΟΤ EXIST
int Change_bloom_filter(bloom_node* first, char* virus, int hash_array[], int num_of_hash);
int check_bloom(bloom_node* first, char* virus, int hash_array[],int num_of_hash);

 
