#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bloom.h"
#include "skip_list.h"

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
    new1->virusName=malloc(strlen(lastName)+1);
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


int push_country(country_node** first,country_node** last ,char* country) {  //INSERT NEW COUNTRY NODE
   
    if(strcmp((*first)->country,"-1")==0){ //IF ITS THE FIRST PERSON
       
      
        (*first)->country=malloc(strlen(country)+1);
     
        strcpy((*first)->country,country);
        return 1;
        
    }
   
    country_node* new = malloc(sizeof(country_node)); //CREATE NEW NODE

    
    new->country=malloc(strlen(country)+1);  
  
    strcpy(new->country,country);
     
    new->next=NULL;
  
    (*last)->next=new;
    (*last)=new;
    return 1;   
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
int Search_for_country( country_node* first,char* country){  //SEARCH FOR COUNTRY

    country_node* current=first;
    while(current!=NULL){
        
        if(strcmp(country,current->country)== 0) //RETURN 1:THE COUNTRY EXIST
            return 1;
        current = current->next;
    }


    return 0; //RETURN 0:THE COUNTRY DOES NOT EXIST
}
///////////////////////////////////////////////////////////////////// HASH FUNCTIONS ////////////////////////////////////////////////////////////////////
/*
This algorithm (k=33) was first reported by dan bernstein many years 
ago in comp.lang.c. 
The magic of number 33 (why it works better than many other constants, 
prime or not) has never been adequately explained.
*/
unsigned long djb2(unsigned char *str) {
	unsigned long hash = 5381;
	int c; 
	while (c = *str++) {
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}
	return hash;
}


/*
This algorithm was created for sdbm (a public-domain reimplementation of ndbm) 
database library. it was found to do well in scrambling bits, causing better 
distribution of the keys and fewer splits. it also happens to be a good 
general hashing function with good distribution. The actual function 
is hash(i) = hash(i - 1) * 65599 + str[i]; what is included below 
is the faster version used in gawk. There is even a faster, duff-device 
version. The magic constant 65599 was picked out of thin air while experimenting 
with different constants, and turns out to be a prime. this is one of the 
algorithms used in berkeley db (see sleepycat) and elsewhere.
*/
unsigned long sdbm(unsigned char *str) {
	unsigned long hash = 0;
	int c;

	while (c = *str++) {
		hash = c + (hash << 6) + (hash << 16) - hash;
	}

	return hash;
}


/* 
Return the result of the Kth hash funcation. This function uses djb2 and sdbm.
None of the functions used here is strong for cryptography purposes but they
are good enough for the purpose of the class.

The approach in this function is based on the paper:
https://www.eecs.harvard.edu/~michaelm/postscripts/rsa2008.pdf
*/
unsigned long hash_i(unsigned char *str, unsigned int i) {
	return djb2(str) + i*sdbm(str) + i*i;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {

    int K=16;
  
    if(argc!=5){
        printf("WRONG INPUT \n");
        return -1;
    }
        
  
    int bloomSize = atoi(argv[4]);
    int max_level=4;
    Skip_List_Node* new2=malloc(sizeof(Skip_List_Node));//CREATE LIST OF SKIP LISTS
    Skip_List_Node* first_skip_list=new2;
    Skip_List_Node* last_skip_list=new2;
    create_list_of_skip_lists(&first_skip_list);

    Skip_List_Node* new3=malloc(sizeof(Skip_List_Node));//CREATE LIST OF SKIP LISTS
    Skip_List_Node* first_not_vaccinated_skip_list=new3;
    Skip_List_Node* last_not_vaccinated_skip_list=new3;
    create_list_of_skip_lists(&first_not_vaccinated_skip_list);


    data_node* new1=malloc(sizeof(data_node));//CREATE LIST OF DATA
    data_node* first_data=new1;
    data_node* last_data=new1;
    new1->next=NULL;
    new1->citizenID="-1"; 
    

    country_node* new4=malloc(sizeof(country_node));//CREATE LIST OF COUNTRIES
    country_node* first_country=new4;
    country_node* last_country=new4;
    new4->next=NULL;
    new4->country="-1"; 

  

    bloom_node* new=malloc(sizeof(bloom_node));//CREATE LIST OF BLOOM FILTERS
    bloom_node* first_bloom=new;
    bloom_node* last_bloom=new;
    new->next=NULL;
    new->v_name="-1"; 
    
    FILE* fd = fopen(argv[2], "r+"); //OPEN THE FILE

    int N = 200; //LINE SIZE
	char line[N] ;	//LINE


    while(fgets(line, N, fd)){ //UNTIL THE END OF THE FILE
            
            
            char* citizenID = strtok(line, " "); //ID NUM
            char* firstName = strtok(NULL, " "); //FIRST NAME
            char* lastName = strtok(NULL, " "); //LAST NAME
            char* country = strtok(NULL, " "); //COUNTRY
            char* age = strtok(NULL, " ");//AGE NUM
            char* virusName = strtok(NULL, " "); //VIRUN NAME
            char* YES_OR_NO = strtok(NULL, " "); //YES OR NO
            char* dateVaccinated = " "; 
            
            if(strcmp(YES_OR_NO,"YES") == 0 )
                dateVaccinated = strtok(NULL, " "); //VACCINATED DATE
           
            if(strcmp(YES_OR_NO,"NO") == 0 ){//IF AFTER NO DOESNT CHANGE LINE
                printf("ERROR IN RECORD %s %s %s %s %s %s %s %s\n",citizenID,firstName,lastName,country,age,virusName,YES_OR_NO,strtok(NULL, " "));
                continue;
            }
            if(Search_for_duplicates_data( first_data, citizenID,  virusName, firstName, lastName, country) == 1){//IF WE HAVE DUPLICATES (SAME ID AND VIRUS)
                printf("ERROR IN RECORD %s %s %s %s %s %s %s %s\n",citizenID,firstName,lastName,country,age,virusName,YES_OR_NO,dateVaccinated);
                continue;
            }
            

            data_node* temp= push_data(&first_data,&last_data , citizenID, firstName, 
            lastName, country, age, virusName, YES_OR_NO, dateVaccinated );//INSERT NEW DATA

            if(Search_for_country(first_country,country)==0)
                push_country(&first_country,&last_country,country);// INSERT NEW COUNTRY

            if(Search_for_virus_bloom(first_bloom,virusName) == 0) //IF THE BLOOM FILTER FOR THE CURRENT VIRUS DOES NΟT EXIST
                push(&first_bloom,&last_bloom,virusName,bloomSize);//INSERT BLOOM FILTER
            

            if(strcmp(YES_OR_NO,"YES") == 0){ //IF THE CURRENT PERSON HAS BEEN VACCINATED

                int hash_array[K];  
                for (int i = 0 ; i < K ; i++) //FIND THE HASH NUMBERS
		            hash_array[i]=hash_i((unsigned char*)citizenID, i)%bloomSize;
                
                Change_bloom_filter(first_bloom,virusName,hash_array,K); //CHANGE THE BLOOM FILTER
            }

            
            if(strcmp(YES_OR_NO,"YES")==0){//IF THE PERSON IS VACCINATED
                
                int flag=0;
                Skip_List_Node* current=first_skip_list;
                while(current!=NULL){ //SEARCH IN VACCINATED SKIP LISTS 
                    if(strcmp(current->v_name,virusName)==0){ // IF THE SKIP LIST EXIST THEN INSERT THE SKIP NODE
                        
                        insert_first_lv_skip(&(current->first_level->first_skip_node),&(current->first_level->last_skip_node),atoi(citizenID),temp);
                        flag=1;
                        break;
                    }
                    current=current->next;
                }
                
                if(flag==0){//IF THE FLAG IS 0 INSERT NEW SKIP LIST FOR THE NEW VIRUS AND NEW SKIP NODR
                    insert_next_skip_list(&first_skip_list,&last_skip_list ,virusName);
                    insert_first_lv_skip(&(last_skip_list->first_level->first_skip_node),&(last_skip_list->first_level->last_skip_node),atoi(citizenID),temp);
                }
            } else{//IF THE PERSON IS NOT VACCINATED
                
                int flag=0;
                Skip_List_Node* current=first_not_vaccinated_skip_list;
                while(current!=NULL){ //SEARCH IN NOT VACCINATED SKIP LISTS
                   
                    if(strcmp(current->v_name,virusName)==0){ // IF THE SKIP LIST EXIST THEN INSERT THE SKIP NODE
                        
                        insert_first_lv_skip(&(current->first_level->first_skip_node),&(current->first_level->last_skip_node),atoi(citizenID),temp);
                        flag=1;
                        break;
                    }
                    current=current->next;
                }
                
                if(flag==0){//IF THE FLAG IS 0 INSERT NEW SKIP LIST FOR THE NEW VIRUS AND NEW SKIP NODE
                
                    insert_next_skip_list(&first_not_vaccinated_skip_list,&last_not_vaccinated_skip_list ,virusName);
                    insert_first_lv_skip(&(last_not_vaccinated_skip_list->first_level->first_skip_node),&(last_not_vaccinated_skip_list->first_level->last_skip_node),atoi(citizenID),temp);
                }

            }
           

    }

    
    

    Skip_List_Node* current=first_skip_list;
    while(current!=NULL){//BUILD VACCINATED SKIP LISTS
        
        build_next_levels( &current->last_level,& current->first_level, max_level);
        current=current->next;
    }
    Skip_List_Node* current1=first_not_vaccinated_skip_list;
    while(current1!=NULL){//BUILD NOT VACCINATED SKIP LIST
        
        build_next_levels( &current1->last_level,& current1->first_level, max_level);
        current1=current1->next;
    }

    
   
while(1){
    

    char cm[200];
    printf("\nGIVE INPUT: ");
    fgets(cm, 200, stdin);
    
    char* x = strtok(cm, " ");

    if(strcmp(x,"/vaccineStatusBloom")==0){//-----------------------------------------/vaccinaStatusBloom ----------------------------------//
        
        char* ID = strtok(NULL, " ");
        char* VN = strtok(NULL, "\n");
        if(VN!=NULL){// ID VIRUS
            int hash_array[K];  
            for (int i = 0 ; i < K ; i++) //FIND THE HASH NUMBERS
                hash_array[i]=hash_i((unsigned char*)ID, i)%bloomSize;
        
            check_bloom( first_bloom, VN, hash_array, K);//MAYBE OR NOT VACCINATED
        }else{
            printf("\nWRONG INPUT\n");
        }

    }else if(strcmp(x,"/vaccineStatus")==0){//---------------------------------------/vaccineStatus --------------------------------------------//
        
      
        char* ID = strtok(NULL, " ");
        char* VN = strtok(NULL, "\n");
        printf("\n");
        if(VN!=NULL){
        
        Skip_List_Node* current=first_skip_list;
        while(current!=NULL){//ID VIRUS
            if(strcmp(current->v_name,VN)==0){// FIND THE SKIP LIST
                check_skip_list(current->first_level, current->last_level, ID, VN);//CHECK FOR THE ID
                break;
            }

            current=current->next;
        }
        if(current==NULL)//IF SKIP LIST WITH THE VIRUS DOESNT EXIST
            printf("\nVIRUS DOESNT EXIST\n");
        }else{ //ID

            Skip_List_Node* current=first_skip_list;
            while(current!=NULL){// FOR EACH VIRUS PRINT IF THE ID HAS VACCINATED OR NOT 
                printf("\n%s\t",current->v_name);
                check_skip_list(current->first_level, current->last_level, ID,current->v_name); 

                current=current->next;
            }
           
        }
            
    }else if(strcmp(x,"/populationStatus")==0){//-------------------------------------- /populationStatus --------------------------------------//

        char* x1 = strtok(NULL, " ");
        char* x2 = strtok(NULL, " ");
        char* x3 = strtok(NULL, " ");
        char* x4 = strtok(NULL, "\n");
        int pop=0;
        

        if(x2==NULL && x3==NULL && x4==NULL){//VIRUS (OR ERROR(COUNTRY))

            x1=strtok(x1,"\n");

            Skip_List_Node* current=first_skip_list;
            country_node* temp=first_country;
            while(temp!=NULL){// ERROR INPUT ONLY COUNTRY
                if(strcmp(temp->country,x1)==0){
                    current=NULL;
                    printf("\nERROR\n");
                    break;
                }
                temp=temp->next;
            }

            while(current!=NULL){
                
                if(strcmp(x1,current->v_name)==0){//FIND THE SKIP LIST
                   
                    country_node* current1=first_country;
                    while(current1!=NULL){//FIND AND PRINT THE POPULATION OF EACH COUNTRY FOR THE CURRENT VIRUS
                        pop=check_population(current->first_level,current1->country, "-1", "-1",x1);
                        printf("\n%s\t%d\t",current1->country,pop);
                        int sum=count_people( first_data,current1->country);
                        if(pop==0)
                            printf("0/100");
                        else
                            printf("%3.1f/100",(float)(100*pop)/sum);

                        current1=current1->next;
                    }
                    
                    break;
                }

                current=current->next;
            }
            printf("\n");


        }else if(x3==NULL && x4==NULL){//COUNTRY VIRUS (OR ERROR(/COUNTRY DATE /VIRUS DATE))
            
            if(atoi(x2)==0){//IF THE SECOND ARGUMENT IS VIRUS
                Skip_List_Node* current=first_skip_list;
                x2=strtok(x2,"\n");
                while(current!=NULL){// FIND AND PRINT THE POPULATION FOR THE CURRENT COUNTY AND VIRUS
            
                    if(strcmp(x2,current->v_name)==0){
                        
                        pop=check_population(current->first_level, x1, "-1", "-1",x2);
                        printf("\n%s\t%d\t",x1,pop);
                        break;
                    }

                    current=current->next;
                }
                int sum=count_people( first_data, x1);
                if(pop==0)
                    printf("0/100\n");
                else
                    printf("%3.1f/100 \n",(float)(100*pop)/sum);
            }else{
                printf("\nERROR\n");
            }

        }else if(x4==NULL){//VIRUS DATE1 DATE2 (OR ERROR(/COUNTRY VIRUS DATE1))
            
            if(strlen(x2) > 11 || strlen(x3)>11){
                printf("\nWRONG INPUT\n");
                continue;
            }
           
            Skip_List_Node* current=first_skip_list;
            country_node* temp=first_country;
            while(temp!=NULL){
                if(strcmp(temp->country,x1)==0){
                    current=NULL;
                    printf("\nERROR\n");
                    break;
                }
                temp=temp->next;
            }
            while(current!=NULL){
                
                if(strcmp(x1,current->v_name)==0){// FIND THE SKIP LIST AND PRINT POPULATION FOR EACH COUNTRY
                   
                    country_node* current1=first_country;
                    while(current1!=NULL){
                        pop=check_population(current->first_level,current1->country, x2, x3,x1);
                        printf("\n%s\t%d\t",current1->country,pop);
                        int sum=count_people( first_data,current1->country);
                        if(pop==0)
                            printf("0/100");
                        else
                            printf("%3.1f/100",(float)(100*pop)/sum);

                        current1=current1->next;
                    }
                    
                    break;
                }

                current=current->next;
            }
            printf("\n");
            

        }else{//COUNTRY VIRUS DATE1 DATE 

            if(strlen(x4) > 11 || strlen(x3)>11){
                printf("\nWRONG INPUT\n");
                continue;
            }
        
            Skip_List_Node* current=first_skip_list;
            while(current!=NULL){

                if(strcmp(x2,current->v_name)==0){
                    pop=check_population(current->first_level, x1, x3, x4, x2);
                    printf("\n%s\t%d\t",x1,pop);
                    break;
                }

                current=current->next;
            }
            int sum=count_people( first_data, x1);
            if(pop==0)
                printf("0/100\n");
            else
                printf("%3.1f/100 \n",(float)(100*pop)/sum);
        }

    }else if(strcmp(x,"/popStatusByAge")==0){//------------------------------------ /popStatusByAge -----------------------------------------//

        int age_array[4]={0,0,0,0};

        char* x1 = strtok(NULL, " ");
        char* x2 = strtok(NULL, " ");
        char* x3 = strtok(NULL, " ");
        char* x4 = strtok(NULL, "\n");
        int pop=0;
        

        if(x2==NULL && x3==NULL && x4==NULL){//VIRUS (OR ERROR(COUNTRY))

            x1=strtok(x1,"\n");

            Skip_List_Node* current=first_skip_list;
            country_node* temp=first_country;
            while(temp!=NULL){// ERROR INPUT ONLY COUNTRY
                if(strcmp(temp->country,x1)==0){
                    current=NULL;
                    printf("\nERROR\n");
                    break;
                }
                temp=temp->next;
            }

            while(current!=NULL){
                
                if(strcmp(x1,current->v_name)==0){//FIND THE SKIP LIST
                   
                    country_node* current1=first_country;
                    while(current1!=NULL){// PRINT POPULATION FOR EACH COUNTRY FOR THE CURRENT VIRUS
                        for(int i=0;i<4;i++)
                            age_array[i]=0;
                        check_population_by_age(current->first_level,current1->country, "-1", "-1",age_array,x1);//RETURN ARRAY WITH WITH COUNTERS BY AGE 
                        printf("\n%s\n",current1->country);
                        for(int i=0; i<4; i++){
                            if(i==0)
                                printf("0-20\t");
                            if(i==1)
                                printf("20-40\t");
                            if(i==2)
                                printf("40-60\t");
                            if(i==3)
                                printf("60+\t");

                            printf("%d\t",age_array[i]);

                            int sum=count_people( first_data,  current1->country );
                            if(age_array[i]==0)
                                printf("0/100\n");
                            else
                                printf("%3.1f/100 \n",(float)(100*age_array[i])/sum);
                            
                        
                        }

                        current1=current1->next;
                    }
                    
                    break;
                }

                current=current->next;
            }
            printf("\n");


        }else if(x3==NULL && x4==NULL){//COUNTRY VIRUS (OR ERROR(/COUNTRY DATE /VIRUS DATE))
            
            if(atoi(x2)==0){
                Skip_List_Node* current=first_skip_list;
                x2=strtok(x2,"\n");
                while(current!=NULL){
                   
                    if(strcmp(x2,current->v_name)==0){//FIND THE SKIP LIST AND PPRINT POPULATION FOR THE CURRENT COUNTRY
                        
                        check_population_by_age(current->first_level, x1, "-1", "-1",age_array,x2);
                        printf("\n%s\n",x1);
                        for(int i=0; i<4; i++){
                            if(i==0)
                                printf("0-20\t");
                            if(i==1)
                                printf("20-40\t");
                            if(i==2)
                                printf("40-60\t");
                            if(i==3)
                                printf("60+\t");

                            printf("%d\t",age_array[i]);

                            int sum=count_people( first_data, x1);
                            if(age_array[i]==0)
                                printf("0/100\n");
                            else
                                printf("%3.1f/100 \n",(float)(100*age_array[i])/sum);
                            
                        
                        }
                        break;
                    }

                    current=current->next;
                }
               
            }else{
                printf("\nERROR\n");
            }

        }else if(x4==NULL){//VIRUS DATE1 DATE2 (OR ERROR(/COUNTRY VIRUS DATE1))
            
           if(strlen(x2) > 11 || strlen(x3)>11){
                printf("\nWRONG INPUT\n");
                continue;
            } 
           
            Skip_List_Node* current=first_skip_list;
            country_node* temp=first_country;
            while(temp!=NULL){//ERROR
                if(strcmp(temp->country,x1)==0){
                    current=NULL;
                    printf("\nERROR\n");
                    break;
                }
                temp=temp->next;
            }
            while(current!=NULL){
                
                if(strcmp(x1,current->v_name)==0){//FIND THE SKIP LIST AND PRINT POPULATION BY AGE FOR EACH COUNTRY
                   
                    country_node* current1=first_country;
                    while(current1!=NULL){
                        for(int i=0;i<4;i++)
                            age_array[i]=0;
                        check_population_by_age(current->first_level,current1->country, x2, x3,age_array,x1);
                        printf("\n%s\n",current1->country);
                        for(int i=0; i<4; i++){
                            if(i==0)
                                printf("0-20\t");
                            if(i==1)
                                printf("20-40\t");
                            if(i==2)
                                printf("40-60\t");
                            if(i==3)
                                printf("60+\t");

                            printf("%d\t",age_array[i]);

                            int sum=count_people( first_data, current1->country);
                            if(age_array[i]==0)
                                printf("0/100\n");
                            else
                                printf("%3.1f/100 \n",(float)(100*age_array[i])/sum);
                            
                        
                        }

                        current1=current1->next;
                    }
                    
                    break;
                }

                current=current->next;
            }
            printf("\n");
            

        }else{//COUNTRY VIRUS DATE1 DATE2 


            if(strlen(x4) > 11 || strlen(x3)>11){
                printf("\nWRONG INPUT\n");
                continue;
            }

            Skip_List_Node* current=first_skip_list;
            while(current!=NULL){

                if(strcmp(x2,current->v_name)==0){//FIND THE SKIP LIST AND PRINT THE POPULATION FOR THE CURRENT COUNTRY BETWEEN TWO DATES
                    check_population_by_age(current->first_level, x1, x3, x4,age_array,x2);
                    printf("\n%s\n",x1);
                    for(int i=0; i<4; i++){
                        if(i==0)
                            printf("0-20\t");
                        if(i==1)
                            printf("20-40\t");
                        if(i==2)
                            printf("40-60\t");
                        if(i==3)
                            printf("60+\t");

                        printf("%d\t",age_array[i]);

                        int sum=count_people( first_data, x1);
                        if(age_array[i]==0)
                            printf("0/100\n");
                        else
                            printf("%3.1f/100 \n",(float)(100*age_array[i])/sum);
                        
                     
                    }
                    
                    break;
                }

                current=current->next;
            }
            
        }

    }else if(strcmp(x,"/insertCitizenRecord")==0){//--------------------------------------/insertCitizenRecord--------------------------------//

        char* ID = strtok(NULL, " ");
        char* first_n = strtok(NULL, " ");
        char* last_n = strtok(NULL, " ");
        char* country = strtok(NULL, " ");
        char* ag = strtok(NULL, " ");
        char* VN = strtok(NULL, " ");
        char* y_n = strtok(NULL, " ");
        char* date = strtok(NULL, "\n");
        

        if(y_n==NULL){
            printf("\nERROR WRHONG INPUT\n");

        }else if(strcmp(y_n,"YES")==0){// IF ITS VACCINATED CITIZENRECORDS
            
            if(strlen(date) > 11 ){
                printf("\nWRONG INPUT\n");
                continue;
            }   

            if(Search_for_virus_bloom(first_bloom,VN) == 0) //IF THE BLOOM FILTER FOR THE CURRENT VIRUS DOES NΟT EXIST
                push(&first_bloom,&last_bloom,VN,bloomSize);

            int hash_array[K];  
            for (int i = 0 ; i < K ; i++) //FIND THE HASH NUMBERS
                hash_array[i]=hash_i((unsigned char*)ID, i)%bloomSize;
            
            Change_bloom_filter(first_bloom,VN,hash_array,K); //CHANGE THE BLOOM FILTER
    

            int flag1=0;
            Skip_List_Node* current1=first_skip_list;
            while(current1!=NULL){ //SEARCH SKIP LISTS
            
                if(strcmp(current1->v_name,VN)==0){ // IF THE SKIP LIST EXIST THEN INSERT THE SKIP NODE
                    flag1=1;
                    break;
                }
                current1=current1->next;
            }

            data_node* data= push_data(&first_data,&last_data , ID, first_n, 
                last_n, country, ag, VN, y_n, date );//RETURN THE DATA NODE
            
            if(Search_for_country(first_country,country)==0)//IF THE COUNTRY DOESNT EXIST INSERT NEW COUNTRY NODE
                    push_country(&first_country,&last_country,country);
            
            if(flag1==1){//IF THE SKIP LIST EXIST FIND THE SKIP LIST AND INSERT THE NEW CITIZEN
                Skip_List_Node* current=first_skip_list;
                while(current!=NULL){
                    if(strcmp(current->v_name,VN)==0)
                        insert_new_citizen(&(current->first_level), &(current->last_level), ID, max_level,data,VN);

                    current=current->next;
                }
                
            } else{//IF THE SKIP LIST DOESNT EXIST INSERT NEW SKIP LIST
                insert_next_skip_list(&first_skip_list,&last_skip_list ,VN);
                insert_first_lv_skip(&(last_skip_list->first_level->first_skip_node),&(last_skip_list->first_level->last_skip_node),atoi(ID),data);
                build_next_levels( &last_skip_list->last_level,& last_skip_list->first_level, max_level);
            }

            Skip_List_Node* current2=first_not_vaccinated_skip_list;
            Skip_List_Node* prev=NULL;
            while(current2!=NULL){
            
                if(strcmp(current2->v_name,VN)==0){
                if(delete_from_not_vaccinated(&(current2->first_level), &(current2->last_level), ID, max_level)==1){
                    prev->next=current2->next;//IF WE WANT TO DELETE THE ONLY NODE IN SKIP LIST WE MUST FREE THE CURRENT SKIP LIST NODE
                    free(current2);
                    break;
                }
                }
                
                prev=current2;
                current2=current2->next;
            }

        }else if(strcmp(y_n,"NO\n")==0){//IF ITS NOT VACCINATED CITIZEN


            int flag1=0;

            Skip_List_Node* current3=first_skip_list;
            while(current3!=NULL){ //SEARCH IN VACCINATED SKIP LISTS
            
                if(strcmp(current3->v_name,VN)==0){ // IF THE SKIP LIST EXIST THEN WE DONT NEED SKIPNODE IN VACCINATED LIST
                    flag1=-1;
                    break;
                }
                current3=current3->next;
            }

            Skip_List_Node* current1=first_not_vaccinated_skip_list;
            while(current1!=NULL){ //SEARCH IN NOT VACCINATED SKIP LISTS
            
                if(strcmp(current1->v_name,VN)==0){ // IF THE SKIP LIST EXIST THEN INSERT THE SKIP NODE
                    flag1=1;
                    break;
                }
                current1=current1->next;
            }

            data_node* data= push_data(&first_data,&last_data , ID, first_n, 
                last_n, country, ag, VN, y_n, " " );//RETURN THE DATA NODE
            
            if(Search_for_country(first_country,country)==0)//IF THE COUNTRY DOESNT EXIST INSERT NEW NODE
                    push_country(&first_country,&last_country,country);
            
            if(flag1==1){//IF THE SKIP LIST EXIST INSERT NEW CITIZEN
                Skip_List_Node* current=first_not_vaccinated_skip_list;
                while(current!=NULL){
                    if(strcmp(current->v_name,VN)==0)
                        insert_new_citizen(&(current->first_level), &(current->last_level), ID, max_level,data,VN);

                    current=current->next;
                }
                
            } else{//IF THE SKIP LIST DOESNT EXIST INSERT NEW SKIP LIST NODE 
                insert_next_skip_list(&first_not_vaccinated_skip_list,&last_not_vaccinated_skip_list ,VN);
                insert_first_lv_skip(&(last_not_vaccinated_skip_list->first_level->first_skip_node),&(last_not_vaccinated_skip_list->first_level->last_skip_node),atoi(ID),data);
                build_next_levels( &last_not_vaccinated_skip_list->last_level,& last_not_vaccinated_skip_list->first_level, max_level);
            }


        }else{
            printf("\nERROR WRHONG INPUT\n");
        }
        
    }else if(strcmp(x,"/vaccinateNow")==0){//------------------------------------ /vaccinateNow -----------------------------------------//

        char* ID = strtok(NULL, " ");
        char* first_n = strtok(NULL, " ");
        char* last_n = strtok(NULL, " ");
        char* country = strtok(NULL, " ");
        char* ag = strtok(NULL, " ");
        char* VN = strtok(NULL, "\n");

        if(VN!=NULL){
            time_t tim = time(NULL);
            struct tm dat = *localtime(&tim);
            char n[11]="";
            char IM[11];
            strcpy(IM,n);
            char day[3];
            char month[3];
            char year[5];
            sprintf(day, "%02d", dat.tm_mday);
            sprintf(month, "%02d", dat.tm_mon+1); 
            sprintf(year, "%d", dat.tm_year+1900); 

            strcat(IM,day);
            strcat(IM,"-");
            strcat(IM,month);
            strcat(IM,"-");
            strcat(IM,year);
            

            if(Search_for_virus_bloom(first_bloom,VN) == 0) //IF THE BLOOM FILTER FOR THE CURRENT VIRUS DOES NΟT EXIST
                push(&first_bloom,&last_bloom,VN,bloomSize);

            int hash_array[K];  
            for (int i = 0 ; i < K ; i++) //FIND THE HASH NUMBERS
                hash_array[i]=hash_i((unsigned char*)ID, i)%bloomSize;
            
            Change_bloom_filter(first_bloom,VN,hash_array,K); //CHANGE THE BLOOM FILTER
    

            int flag1=0;
            Skip_List_Node* current1=first_skip_list;
            while(current1!=NULL){ //SEARCH SKIP LISTS
            
                if(strcmp(current1->v_name,VN)==0){ // IF THE SKIP LIST EXIST THEN INSERT THE SKIP NODE
                    flag1=1;
                    break;
                }
                current1=current1->next;
            }

            data_node* data= push_data(&first_data,&last_data , ID, first_n, 
                last_n, country, ag, VN, "YES", IM );//RETURN DATA NODE
            
            if(Search_for_country(first_country,country)==0)//IF THE COUNTRY DOESNT EXIST INSERT NEW COUNTRY NODE
                    push_country(&first_country,&last_country,country);
            
            if(flag1==1){//IF THE SKIP LIST EXIST INSER ENW CITIZEN
                Skip_List_Node* current=first_skip_list;
                while(current!=NULL){
                    if(strcmp(current->v_name,VN)==0)
                        insert_new_citizen(&(current->first_level), &(current->last_level), ID, max_level,data,VN);

                    current=current->next;
                }
                
            } else{//IF THE SKIP LIST DOESNT EXIST INSERT NEW SKIP LIST 
                insert_next_skip_list(&first_skip_list,&last_skip_list ,VN);
                insert_first_lv_skip(&(last_skip_list->first_level->first_skip_node),&(last_skip_list->first_level->last_skip_node),atoi(ID),data);
                build_next_levels( &last_skip_list->last_level,& last_skip_list->first_level, max_level);
            }

            Skip_List_Node* current2=first_not_vaccinated_skip_list;
            Skip_List_Node* prev=NULL;
            while(current2!=NULL){
               
                if(strcmp(current2->v_name,VN)==0){//IF THE CURRENT ID IS IN THE NOT VACCINATED SKIP LIST
                   if(delete_from_not_vaccinated(&(current2->first_level), &(current2->last_level), ID, max_level)==1){
                       prev->next=current2->next;
                       free(current2);// IF WE MUST TO DELETE THE SKIP LISTI
                       break;
                   }
                }
                
                prev=current2;
                current2=current2->next;
            }

        }else{
           printf("\nERROR WRHONG INPUT\n"); 
        }

    }else if(strcmp(x,"/list-nonVaccinated-Persons")==0){//--------------------/list-nonVaccinated-Persons --------------------//

        char* VN = strtok(NULL, "\n");
        if(VN!=NULL){
            Skip_List_Node* current=first_not_vaccinated_skip_list;
            while(current!=NULL){
                if(strcmp(VN,current->v_name)==0){//IF THE SKIP LIST EXIST PRINT THE DATA FOR EACH CITIZEN
                    Skip_Node* temp=current->first_level->first_skip_node;
                    while(temp!=NULL){
                        printf("\n%s\t%s\t%s\t%s\t%s\n",temp->data->citizenID,temp->data->firstName,temp->data->lastName,temp->data->country,temp->data->age);
                        temp=temp->next;
                    }
                }

                current=current->next;
            }
        }else{
            printf("\nWRONG INPUT\n");
        }
    }else if(strcmp(x,"/exit\n")==0){


        //FREE VACCINATED SKIP LIST
        Skip_List_Node* current=first_skip_list;
        first_skip_list=NULL;
        last_skip_list=NULL;
        while(current!=NULL){
            
            Lv_Node* current1=current->last_level;
            current->first_level=NULL;
            current->last_level=NULL;
            while(current1!=NULL){
                
                Skip_Node* current2=current1->first_skip_node;
                current1->first_skip_node=NULL;
                current1->last_skip_node=NULL;
                while(current2!=NULL){
                    
                   Skip_Node* temp2=current2;
                   current2=current2->next;

                   temp2->data=NULL;
                   temp2->down=NULL;
                   temp2->next=NULL;
                   free(temp2);
                }
                
                Lv_Node* temp1=current1;
                current1=current1->down;

                if(current1!=NULL)
                    current1->up=NULL;
                
                temp1->down=NULL;
                temp1->up=NULL;
                free(temp1);
            }
            
            Skip_List_Node* temp=current;
            current=current->next;

            temp->next=NULL;
            free(temp->v_name);
            free(temp);

        }

        //FREE NOT VACCINATED SKIP LIST
        Skip_List_Node* n_current=first_not_vaccinated_skip_list;
        first_not_vaccinated_skip_list=NULL;
        last_not_vaccinated_skip_list=NULL;
        while(n_current!=NULL){
            
            Lv_Node* n_current1=n_current->last_level;
            n_current->first_level=NULL;
            n_current->last_level=NULL;
            while(n_current1!=NULL){
                
                Skip_Node* n_current2=n_current1->first_skip_node;
                n_current1->first_skip_node=NULL;
                n_current1->last_skip_node=NULL;
                while(n_current2!=NULL){
                    
                   Skip_Node* n_temp2=n_current2;
                   n_current2=n_current2->next;

                   n_temp2->data=NULL;
                   n_temp2->down=NULL;
                   n_temp2->next=NULL;
                   free(n_temp2);
                }
                
                Lv_Node* n_temp1=n_current1;
                n_current1=n_current1->down;

                if(n_current1!=NULL)
                    n_current1->up=NULL;
                
                n_temp1->down=NULL;
                n_temp1->up=NULL;
                free(n_temp1);
            }
            
            Skip_List_Node* n_temp=n_current;
            n_current=n_current->next;

            n_temp->next=NULL;
            free(n_temp->v_name);
            free(n_temp);

        }

        //FREE COUNTRY LIST
        country_node* c_current=first_country;
        first_country=NULL;
        last_country=NULL;

        while(c_current!=NULL){
            country_node* c_temp=c_current;
            c_current=c_current->next;

            free(c_temp->country);
            c_temp->next=NULL;
            free(c_temp);  
        }

        //FREE DATA LIST
        data_node* d_current=first_data;
        first_data=NULL;
        last_data=NULL;

        while(d_current!=NULL){
            
            data_node* d_temp=d_current;
            d_current=d_current->next;

            free(d_temp->citizenID);
            free(d_temp->firstName);
            free(d_temp->lastName);
            free(d_temp->country);
            free(d_temp->age);
            
            virus* v_current=d_temp->v_name;

            d_temp->v_name=NULL;
            while(v_current!=NULL){
                
                virus* v_temp=v_current;
                v_current=v_current->next;

                free(v_temp->date);
                free(v_temp->virusName);
                free(v_temp->y_n);
                v_temp->next=NULL;
                free(v_temp);
            }
            d_temp->next=NULL;
            free(d_temp);  
        }

        //FREE BLOOM FILTER LIST

        bloom_node* b_current=first_bloom;
        first_bloom=NULL;
        last_bloom=NULL;
       
        while(b_current!=NULL){
            
            bloom_node* b_temp=b_current;
            b_current=b_current->next;
            
            free(b_temp->bloom);
            free(b_temp->v_name);
            b_temp->next=NULL;

            free(b_temp);  
        }



        fclose(fd);
        return 1;

    }else{
        printf("\nWRONG INPUT\n");
    }
    
}

}
