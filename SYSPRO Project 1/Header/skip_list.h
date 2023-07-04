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
///// COUNTRY
typedef struct country_node {

    char* country;
    struct country_node* next;
    
} country_node;

///// SKIP LIST
typedef struct Skip_List_Node {
    char* v_name;
    struct Lv_Node *first_level;
    struct Lv_Node *last_level;
    struct Skip_List_Node *next;
    
} Skip_List_Node;

typedef struct Skip_Node {
    int key;
    struct data_node *data;
    struct Skip_Node *next;
    struct Skip_Node *down;
} Skip_Node;

typedef struct Lv_Node {
    int level;
    struct Lv_Node *up;
    struct Lv_Node *down;
    struct Skip_Node *first_skip_node;
    struct Skip_Node *last_skip_node;
    
} Lv_Node;


int insert_first_lv_skip(Skip_Node** first,Skip_Node** last,int x, data_node* data);
int build_next_levels( Lv_Node** last,Lv_Node** level_zero,int max);
int create_list_of_skip_lists(Skip_List_Node** first );
int insert_next_skip_list(Skip_List_Node** first,Skip_List_Node** last ,char* virus);
int check_skip_list(Lv_Node* first, Lv_Node* last, char* id,char* vi);
int compare_dates(char* d,char* d1,char* d2);
int check_population(Lv_Node* first, char* country, char*date1, char*date2,char* vi);
int count_people(data_node* first, char* country);
int check_population_by_age(Lv_Node* first, char* country, char*date1, char*date2,int* age,char* vi);
int insert_new_citizen(Lv_Node** first, Lv_Node** last, char* id,int lv_size,data_node* data,char* vi);
int delete_from_not_vaccinated(Lv_Node** first, Lv_Node** last, char* id,int lv_size);