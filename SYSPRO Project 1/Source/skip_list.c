#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include"skip_list.h"
#include<time.h>



int insert_first_lv_skip(Skip_Node** first,Skip_Node** last,int x, data_node* data) {

    

    if((*first)->key==-1){ //IF ITS THE FIRST SKIP NODE
        (*first)->key=x;
        (*first)->data=data;
        return 1;
        
    }

    if((*first)->key > x){ //IF THE NEW KEY IS LOWER THAN THE FIRST 
        Skip_Node* new = malloc(sizeof(Skip_Node));
        new->key=x;
        new->data=data;
        new->down=NULL;
        new->next=(*first);
        (*first)=new;
        return 1;
    }

    Skip_Node* current=(*first);
    Skip_Node* prev=NULL;

    while(current->next!=NULL){ 
        if(current->key>x){ //IF THE NEW KEY IS LOWER THAN THE CURRENT 
            Skip_Node* new = malloc(sizeof(Skip_Node));
            new->key=x;
            new->data=data;
            new->down=NULL;
            new->next=current;
            prev->next=new;
            return 1;
        }
        prev = current;
        current=current->next;
    }

    if(current->key>x){ //IF THE NEW KEY IS LOWER THAN THE LAST

        Skip_Node* new = malloc(sizeof(Skip_Node));
        new->key=x;
        new->data=data;
        new->down=NULL;
        new->next=current;
        prev->next=new;
        return 1;

    }else if(current->key<x){ //IF THE NEW KEY IS HIGHER THAN THE LAST

        Skip_Node* new = malloc(sizeof(Skip_Node));
        new->key=x;
        new->data=data;
        new->down=NULL;
        new->next=NULL;
        current->next=new;
        (*last)=new;
        return 1;
    }

    return 0;   
} 
int build_next_levels( Lv_Node** last,Lv_Node** level_zero,int max){
    
    
    Lv_Node* current_level=(*level_zero);
    Skip_Node* current_skip=(*level_zero)->first_skip_node;
    int counter=1;
    srand(time(NULL));
    while(counter<= max){ //CREATE UNTIL MAX LEVEL
        int first_skip_flag=0;//FLAG=0 IF ITS THE FIRST SKIP LIST NODE IN THE NEXT LEVEL
        
        
        while(current_skip!=NULL){//CHECK EVERY SKIP NODE

            int r = rand() % 2 + 1;//FLIP A COIN
            
            if(r==1 && first_skip_flag==0){//IF ITS THE FIRST SKIP NODE IN THE NEXT LEVEL

                Skip_Node* new1= malloc(sizeof(Skip_Node));//CREATE SKIP NODE
                new1->down=current_skip; 
                new1->key=current_skip->key;
                new1->data=current_skip->data;
                new1->next=NULL;

                Lv_Node* new=malloc(sizeof(Lv_Node));//CREATE LEVEL NODE
                new->down=current_level;
                new->up=NULL;
                new->level=counter;

                new->first_skip_node=new1;
                new->last_skip_node=new1;
                (*last)=new;
                current_level->up=new;
                first_skip_flag=1;
                

            }else if(r==1){ //IF THE NRE NODE IN THE NEXT LEVEL ITS NOT THE FIRST
                
                Skip_Node* new1= malloc(sizeof(Skip_Node));//CREATE SKIP NODE
                new1->down=current_skip;
                new1->key=current_skip->key;
                new1->data=current_skip->data;
                new1->next=NULL;
                
                current_level->up->last_skip_node->next=new1;
                current_level->up->last_skip_node=new1;

            }
            
            current_skip=current_skip->next;//NEXT SKIP NODE
        }
       
        if(current_level->up==NULL)//IF WE DONT NEED NEW LEVEL
            return 1;

        current_level=current_level->up;//NEXT LEVEL
        current_skip=current_level->first_skip_node;//NEXT SKIP LIST
        
        counter++;
        
    }
}
int create_list_of_skip_lists(Skip_List_Node** first ){

    Skip_Node* new1= malloc(sizeof(Skip_Node));
    new1->down=NULL;
    new1->key=-1;
    new1->next=NULL;

    Lv_Node* new=malloc(sizeof(Lv_Node));
    new->down=NULL;
    new->up=NULL;
    new->level=0;

    new->first_skip_node=new1;
    new->last_skip_node=new1;

    (*first)->first_level =new;
    (*first)->last_level =new;
    (*first)->next=NULL;
    (*first)->v_name="-1";

    return 1;
    
}

int insert_next_skip_list(Skip_List_Node** first,Skip_List_Node** last ,char* virus){
    
    if(strcmp((*first)->v_name,"-1")==0){ //IF ITS THE FIRST VIRUS
        (*first)->v_name=malloc(strlen(virus)+1);
        strcpy((*first)->v_name,virus);
        return 1;
        
    }
    
    
    Skip_List_Node* new=malloc(sizeof(Skip_List_Node)); //CREATE NEW NODE
    Skip_Node* new1= malloc(sizeof(Skip_Node));
    new1->down=NULL;
    new1->key=-1;
    new1->next=NULL;
    
    Lv_Node* new2=malloc(sizeof(Lv_Node));
    new2->down=NULL;
    new2->up=NULL;
    new2->level=0;

    new2->first_skip_node=new1;
    new2->last_skip_node=new1;

    new->first_level =new2;
    new->last_level =new2;
    new->next=NULL;
    
    new->v_name=malloc(strlen(virus)+1);
    strcpy(new->v_name,virus);

    (*last)->next=new;
    (*last)=new;

    return 1;

}

int check_skip_list(Lv_Node* first, Lv_Node* last, char* id,char* vi){
    
    Lv_Node* current= last;
    Skip_Node* first_skip=current->first_skip_node;
    Skip_Node* prev=NULL;

    while(current!=NULL){//START FROM LAST LEVEL
       
        Skip_Node* currents=first_skip;
        Skip_Node* current_prev=prev;
        while(currents!=NULL){//SEARCH IN CURRENT LEVEL LIST
            if(atoi(id)==currents->key){//IF YOU FIND  THE ID
                virus* v=currents->data->v_name;
                while(v!=NULL){
                    if(strcmp(v->virusName,vi)==0)//PRINT HE DATE FOR THE CURRENT VIRUS
                        printf("VACCINATED ON %s\n",v->date);

                    v=v->next;
                }

                
                return 1;
            }
            if(atoi(id)<currents->key){//IF THE KEY LOWER THAN THE CURRENT
               
                if(currents==current->first_skip_node){
                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }
                    current=current->down;
                    first_skip=current->first_skip_node;
                    break;
                }else{
                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }
                    current=current->down;
                    first_skip=current_prev->down;
                    prev=NULL;
                    break;
                }
            }
            if(atoi(id)>currents->key){//IF THE KEY IS HIGHER THAN THE CURRENT
                if(currents==current->last_skip_node){
                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }
                    current=current->down;
                    first_skip=currents->down;
                    prev=NULL;
                    break;
                }else{
                    current_prev=currents;
                    currents=currents->next;
                }
            }
        }

    }
    printf("NOT VACCINATED \n");
    return 0;
}

int compare_dates(char* d,char* d1,char* d2){
    
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

int count_people(data_node* first, char* country){
    int counter=0;

    data_node* current=first;

    while(current!=NULL){//COUNT THE CITIZENS FROM THE COUNTRY
        if(strcmp(current->country,country)==0)
            counter++;

        current=current->next;
    }
    return counter;
    
}

int check_population(Lv_Node* first, char* country, char*date1, char*date2, char*vi){

    int counter=0;
    Skip_Node* current=first->first_skip_node;

    while(current!=NULL){//SEARCH IN LEVEL 0 FOR CITIZENS FROM THE CURRENT COUNTRY

        if(strcmp(current->data->country,country)==0){ 
            if(strcmp(date1,"-1")==0 && strcmp(date2,"-1")==0)//WITHOUT DATES 
                counter++;
            else{ 
                virus* v=current->data->v_name;
                while(v!=NULL){
                    if(strcmp(v->virusName,vi)==0)
                       if(compare_dates(v->date,date1,date2)==1){//WITH DATES 
                            counter++;
                            break;
                       }
                    v=v->next;
                }
                
               
            }
        }
         current=current->next;
    }
    return counter;

}
int check_population_by_age(Lv_Node* first, char* country, char*date1, char*date2,int* age,char* vi){

    Skip_Node* current=first->first_skip_node;

    while(current!=NULL){//SEARCH IN LEVEL 0 FOR THE CITIZEN IN THE CURRENT COUNTRY

        if(strcmp(current->data->country,country)==0)
            if(strcmp(date1,"-1")==0 && strcmp(date2,"-1")==0){//WITHOUT DATES
                if(atoi(current->data->age)<=20)//0-20
                    age[0]++;
                else if(atoi(current->data->age)<=40)//20-40
                    age[1]++;
                else if(atoi(current->data->age)<=60)//40-60
                    age[2]++;
                else if(atoi(current->data->age)>60)//60+
                    age[3]++;
            }else{ // WITH DATES
                
                virus* v=current->data->v_name;
                while(v!=NULL){
                    if(strcmp(v->virusName,vi)==0){
                        if(compare_dates(v->date,date1,date2)==1){
                            if(atoi(current->data->age)<=20)
                                age[0]++;
                            else if(atoi(current->data->age)<=40)
                                age[1]++;
                            else if(atoi(current->data->age)<=60)
                                age[2]++;
                            else if(atoi(current->data->age)>60)
                                age[3]++;
                        }
                        break;
                    }
                    v=v->next;
                }
                
                
            } 
        
         current=current->next;
    }

    return 1;

}
int insert_new_citizen(Lv_Node** first, Lv_Node** last, char* id,int lv_size,data_node* data,char*vi){

    Skip_Node* prev_nodes[lv_size+1];
    Skip_Node* next_nodes[lv_size+1];
    for(int i=0;i<lv_size+1;i++){
        prev_nodes[i]=NULL;
        next_nodes[i]=NULL;
    }

    Lv_Node* current= (*last);
    Skip_Node* first_skip=current->first_skip_node;
    Skip_Node* prev=NULL;

    while(current!=NULL){//SEARCH FROM LAST LEVEL 

        Skip_Node* currents=first_skip;
        Skip_Node* current_prev=prev;
        while(currents!=NULL){//SEARCH IN CURRENT LEVEL LIST
           
            if(atoi(id)==currents->key){//CITIZEN ALREADY EXIST
                virus* v=currents->data->v_name;
                while(v!=NULL){
                    if(strcmp(v->virusName,vi)==0){
                        if(strcmp(v->y_n,"YES")==0)
                            printf("\nERROR: CITIZEN %s ALREADY VACCINATED ON %s\n",id,v->date);
                        else 
                            printf("\nERROR:CITIZEN %s ALREADY IN NOT VACCINATED LIST\n",id);
                        return 1;
                    }
                    v=v->next;
                } 
                
            }
            if(atoi(id)<currents->key){//IF THE NEW IS BEFOR THE CURRENT 
                if(currents==current->first_skip_node){//IF THE NEW IS BEFOR THE FIRST NODE
                    
                    prev_nodes[current->level]=NULL;
                    next_nodes[current->level]=currents;
                    
                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }
                        
                    current=current->down;
                    
                    first_skip=current->first_skip_node;
                    
                    break;
                }else{
                  
                    prev_nodes[current->level]=current_prev;
                    next_nodes[current->level]=currents;

                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }

                    current=current->down;
                    first_skip=current_prev->down;
                    prev=NULL;
                    break;
                }
            }
            if(atoi(id)>currents->key){//IF THE NEW IS AFTER THE CURRENT
                if(currents==current->last_skip_node){//IF THE CURRENT IS THE LAST
                    
                    prev_nodes[current->level]=currents;
                    next_nodes[current->level]=NULL;

                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }

                    current=current->down;
                    first_skip=currents->down;
                    prev=NULL;
                    break;
                }else{
                    
                    current_prev=currents;
                    currents=currents->next;
                }
            }
        }

    }

    srand(time(NULL));
    int r=1;
    Skip_Node* down_node=NULL;
    for(int i=0; i<lv_size+1; i++){
        
        if(r==1){
            
            if(next_nodes[i]==NULL && prev_nodes[i]==NULL){//IF WE NEED NEW LEVEL
                
                Skip_Node* new= malloc(sizeof(Skip_Node));//CREATE SKIP NODE
                new->down=down_node; 
                new->key=atoi(id);
                new->data=data;
                new->next=NULL;

                Lv_Node* new1=malloc(sizeof(Lv_Node));//CREATE LEVEL NODE
                new1->down=(*last); 
                new1->up=NULL;
                new1->level=((*last)->level)+1;

                new1->first_skip_node=new;
                new1->last_skip_node=new;
                (*last)->up=new1;
                (*last)=new1;
                
            }else if(prev_nodes[i]==NULL){//IF THE NEW NODE IS THE FIRST
                
                Skip_Node* new=malloc(sizeof(Skip_Node));
                new->key=atoi(id);
                new->data=data;
                new->down=down_node;
                new->next=next_nodes[i];

                Lv_Node* temp=(*first);
                while(temp!=NULL){
                    if(temp->level==i)
                        temp->first_skip_node=new;
                    temp=temp->up;  
                }
                down_node=new;

            }else if(next_nodes[i]==NULL){//IF THE NEW NODE IS THE LAST
                
                Skip_Node* new=malloc(sizeof(Skip_Node));
                new->key=atoi(id);
                new->data=data;
                new->down=down_node;
                new->next=next_nodes[i];
                prev_nodes[i]->next=new;

                Lv_Node* temp=(*first);
                while(temp!=NULL){
                    if(temp->level==i)
                        temp->last_skip_node=new;
                    temp=temp->up;  
                }
                down_node=new;
            }else{//BETWEEN TWO NODES
               
                Skip_Node* new=malloc(sizeof(Skip_Node));
                new->key=atoi(id);
                new->data=data;
                new->down=down_node;
                new->next=next_nodes[i];
                prev_nodes[i]->next=new;

                down_node=new;
            }
            
        }else{
            break;
        }
        r = rand() % 2 + 1;
    }



    return 0;
}

int delete_from_not_vaccinated(Lv_Node** first, Lv_Node** last, char* id,int lv_size){
   
    int lv=-1;
    Skip_Node* prev_nodes[lv_size+1];
    Skip_Node* next_nodes[lv_size+1];
   
    for(int i=0;i<lv_size+1;i++){
        prev_nodes[i]=NULL;
        next_nodes[i]=NULL;
    }
    
    Lv_Node* current= (*last);
    Skip_Node* first_skip=current->first_skip_node;
    Skip_Node* prev=NULL;

    while(current!=NULL){//SEARCH FROM LAST LEVEL 

        Skip_Node* currents=first_skip;
        Skip_Node* current_prev=prev;
        while(currents!=NULL){
           
            if(atoi(id)==currents->key){//CITIZEN ALREADY EXIST
                if(lv==-1){
                    lv=current->level;//THE LEST LEVEL THAT THE NODE EXIST
                }
                if(currents==current->first_skip_node){//IF ITS THE FIRST

                    prev_nodes[current->level]=NULL;
                    next_nodes[current->level]=currents->next;
                   
                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }     
                    current=current->down;
                    first_skip=current->first_skip_node;
                    
                    break;

                }if(currents==current->last_skip_node){//IF ITS LAST

                    prev_nodes[current->level]=current_prev;
                    next_nodes[current->level]=NULL;
                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }

                    current=current->down;
                    first_skip=current_prev->down;
                    prev=NULL;;
                    break;
                }else{
                  
                    prev_nodes[current->level]=current_prev;
                    next_nodes[current->level]=currents->next;
                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }

                    current=current->down;
                    first_skip=current_prev->down;
                    prev=NULL;
                    break;
                }
               
            }
            if(atoi(id)<currents->key){//IF THE NODE IS BEFOR THE CURRENT 
                if(currents==current->first_skip_node){//IF THE NODE IS BEFOR THE FIRST NODE
                    
                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }
                        
                    current=current->down;
                    
                    first_skip=current->first_skip_node;
                    
                    break;
                }else{

                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }

                    current=current->down;
                    first_skip=current_prev->down;
                    prev=NULL;
                    break;
                }
            }
            if(atoi(id)>currents->key){//IF THE NODE IS AFTER THE CURRENT
                if(currents==current->last_skip_node){//IF THE CURRENT IS THE LAST NODE

                    if(current->down==NULL){
                        current=NULL;
                        break;
                    }

                    current=current->down;
                    first_skip=currents->down;
                    prev=NULL;
                    break;
                }else{
                    
                    current_prev=currents;
                    currents=currents->next;
                }
            }
        }

    }
   
    int f=0;
    if(lv==-1)//THE NODE DOESNT EXIST
        return 0;
    
    for(int i=lv; i>=0; i--){// FROM THE LAST NODE TO THE FIRST
       
        if(prev_nodes[i]==NULL && next_nodes[i]==NULL){//IF ITS THE ONLY NODE IN CURRENT LEVEL
            Lv_Node* temp=(*last);
            
            while(temp!=NULL){
                if(temp->level==i){
                    temp->first_skip_node->data=NULL;
                    temp->first_skip_node->down=NULL;
                    temp->first_skip_node->next=NULL;
                    
                    Skip_Node* temp1=temp->first_skip_node;
                    (*last)=temp->down;
                    
                    temp->first_skip_node=NULL;
                    temp->last_skip_node=NULL;
                  
                    if(temp->down!=NULL)
                        temp->down->up=NULL;
                    else f=1;
                    
                    temp->up==NULL;
                    temp->down=NULL;
                    free(temp1);//FREE NODE
                    free(temp);//FREE LEVEL
                    
                    break;
                }
                temp=temp->down;  
            } 
            
        }else if(prev_nodes[i]==NULL){//IF THE NODE IS THE FIRST
           
            Lv_Node* temp=(*first);
            while(temp!=NULL){
                if(temp->level==i){
                    temp->first_skip_node->data=NULL;
                    temp->first_skip_node->down=NULL;
                    temp->first_skip_node->next=NULL;
                    
                    Skip_Node* temp1=temp->first_skip_node;
                    temp->first_skip_node=next_nodes[i];
                    free(temp1);//FREE NODE
                    break;
                }
                temp=temp->up;  
            }

        }else if(next_nodes[i]==NULL){//IF THE NODE IS THE LAST
      
        Lv_Node* temp=(*first);
        while(temp!=NULL){
            if(temp->level==i){
                temp->last_skip_node->data=NULL;
                temp->last_skip_node->down=NULL;
                temp->last_skip_node->next=NULL;
                
                Skip_Node* temp1=temp->last_skip_node;
                temp->last_skip_node=prev_nodes[i];
                prev_nodes[i]->next=NULL;
                free(temp1);//FREE NODE
                break;
            }
            temp=temp->up;  
        }

        }else{//IF ITS BETWEEN TOW NODES
            
            Skip_Node* temp1=prev_nodes[i]->next;
            temp1->next=NULL;
            temp1->down=NULL;
            temp1->data=NULL;
            prev_nodes[i]->next=next_nodes[i];
            free(temp1);//FREE NODE
        }  
    }
    

 


    if(f==1)// ΙF THE F IS 1 THEN DELETE THE SKIP LIST NODE
        return 1;
    else
        return 0;
}