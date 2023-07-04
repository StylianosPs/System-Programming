#include <stdio.h>
#include <stdlib.h>
#pragma once
#include <string.h>
#include<time.h>

///// DATA
typedef struct virus {

    char* virusName;
    char* y_n;
    char* date;
    struct virus* next;
    
} virus;
typedef struct data_node {

    char* citizenID;
    char* firstName; 
    char* lastName;
    char* country;
    char* age;
    virus* v_name;
    
    struct data_node* next;
    
} data_node;


//COUNTRY FILES
typedef struct country_files {

    char* file;
    struct country_files* next;
    
} country_files;

data_node* push_data(data_node** first,data_node** last ,char* citizenID, char* firstName, 
char* lastName, char* country, char* age, char* virusName, char* YES_OR_NO, char* dateVaccinated );
int Search_for_duplicates_data( data_node* first,char* id,  char* virus_n, char* f_name, char* l_name, char* country);
int travelRequest(data_node* first, char* id, char* v_name,char** ans,int* accepted, int* rejected );
int push_country_files(country_files** first, country_files** last, char* file);
int SearchVaccinationStatus(data_node* first, char* ID, char** output );
int logfile(int accepted, int rejected, char* countries);