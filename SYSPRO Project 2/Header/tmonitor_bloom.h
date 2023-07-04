#include <stdlib.h>
#pragma once


///// BLOOM
typedef struct bloom_filters {
    char* virus;
    char* bloom;
    struct bloom_filters* next;
    
} bloom_filters;
typedef struct countries {

    char* country;
    struct countries* next;
    
} countries;

typedef struct monitors_bloom {

    int monitor;
    countries* f_country;
    bloom_filters* f_bloom;
    
    struct monitors_bloom* next;
    

} monitors_bloom;
////// REQUESTS
typedef struct dates{

    char* virus;
    char* date;
    int ac_re;
    struct dates* next;
    
} dates;
typedef struct request_node{

    char* country;
    struct dates* f_date;
    struct request_node* next;
    
} request_node;



int push_country(monitors_bloom** first,monitors_bloom** last ,int mon,char* country);
int push_bloom(monitors_bloom** first,monitors_bloom** last,int mon ,char* virus ,char* bloom);
int search_blooms(monitors_bloom* first,char* countryfrom, char* virus, int hash_array[],int num_of_hash);
int find_monitor(monitors_bloom* first, char* country);
int compare_dates(char* d1,char* d2);
int push_request(request_node** first, request_node** last, char* country, char* date, char* virus, int ac_re);
int travelStatus(request_node* first, char* country,char* virus, char* date1, char* date2);
int compare_dates_for_status(char* d,char* d1,char* d2);
int update_bloom(monitors_bloom** first,monitors_bloom** last,int mon ,char* virus ,char* bloom);
int logfile(request_node* first_requesr, monitors_bloom* first_bloom);


