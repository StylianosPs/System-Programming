#include <stdio.h>
#include <stdlib.h>
#pragma once
#include <string.h>
#include "data.h"
#include "bloom.h"

//NODE WITH CYCLICBUFFER
typedef struct file_buffer {
    char** file;//CYCLICBUFFER
    int first;
    int last;
    int size;
    int counter;
} file_buffer;

//NODE WITH ARGS FOR THREADS
typedef struct thread_args {
    data_node* first_data;
    data_node* last_data;

    bloom_node* first_bloom;
    bloom_node* last_bloom;

    file_buffer* file_buffer_node;

    int bloomSize;
    
} thread_args;


int initialize_file_buffer(file_buffer* node, int cyclicbuffersize);
int add_file(file_buffer* node, char* file);
int put_records_in_structurs(data_node** first_data, data_node** last_data, bloom_node** first_bloom, bloom_node** last_bloom, char* final_name, int bloomSize );