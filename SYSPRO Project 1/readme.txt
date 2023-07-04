Stylianos Psara sdi1800226

bloom.c
 Gia tin domi tou bloom filter xrisimopio mia lista h opoia periexei to onoma tou iou kai ena deikth ston opoio desmevo mnhmh gia to bloom filter

#int push(bloom_node** first,bloom_node** last ,char* virus, int bloomSize )
Dimiourgei ena komvo bloom_node arxikopoiei to bloom filter kai ton prosthetei sto telos ths listas
#int Search_for_virus_bloom( bloom_node* first, char* virus)
Psaxnei an iparxei komvos gia ton io pou pairnei san orisma kai an iparxei epistrefei 1 alios epistrefei 0
#int Change_bloom_filter(bloom_node* first, char* virus, int hash_array[], int num_of_hash)
Vriskei ton komvo tou iou pou pairnei san orisma kai anathetei thn timh 1 stis theseis tou pinaka pou pairnei apo ton hash_array
#int check_bloom(bloom_node* first, char* virus, int hash_array[],int num_of_hash)
Elegxei an oi theseis tou bloom filter pou pairnei apo to hash_array periexoun ena toulaxisto 0 kai tiponei to minima NOT VACCINATED alios an einai ola 1 tiponei MAYBE

skip_list.c
Gia thn domh toy bloom filter zrhsimopoihsa mia lista me kombous Skip_ListNode oi opoioi periexoun to onoma tou iou kai dio deiktes ena pou deixnei sto proto kai ena sto telefteo komvo  lv_node tou skip liat. Oi komvoi lv_node periexoun level kai dio diktes Skip_Node gia thn arxh kai to telos tis listas me ta stoixeia tou trexon epipedou. Oi komvoi Skip_Node periexoun to kleidi diladi to id kai ena dikti sthn lista me ta stoixeia citizen.

#int insert_first_lv_skip(Skip_Node** first,Skip_Node** last,int x, data_node* data)
Auth h sinartisi prosthetei ena kainourgio komv level 0 tis Skip list analoga me thn periptosi.
#int build_next_levels( Lv_Node** last,Lv_Node** level_zero,int max)
Gia kathe komvo sto epipedo 0 apofasizei tixea an xriazetai komvos sto epomeno epipedo kai an xreiastei ton prosthetei kai molis teliosei apo olous tous komvous level 0 epanalamvanei tin diadikasia gia to epomeno. H diadikasia auti teratizei otan den xreiazetai na dimiourgithei epomeno epipedo h otan I skip list ftasei to megisto epipedo.
#int create_list_of_skip_lists(Skip_List_Node** first )
Dimiourgei ton proto komvo stin lista me ta skip list.
#int insert_next_skip_list(Skip_List_Node** first,Skip_List_Node** last ,char* virus)
Prosthetei ton epomeno kombo stin lista meta skip lists.
#int check_skip_list(Lv_Node* first, Lv_Node* last, char* id,char* vi)
Auti I sinartisi psaxnei apo to telefteo mexri to proto epipedo to id kai analoga me tin periptosi allazei epipedo mexri na vrei to id kai na tiposi tin imerominia toy emvoliasmoy alios an den to vrei tipo ei NOT  VACCINATED.
#int compare_dates(char* d,char* d1,char* d2)
Pernei 3 imerominies tis xorizei se imera mina xrono kai tis sigkrinei an I imeromneia d einai anamesa sthn d1 kai d2 tote epistredei 1.
#int count_people(data_node* first, char* country)
Metraei apo tin lista data tous citizens pou einai apo tin xora pou pairnei san orisma.
#int check_population(Lv_Node* first, char* country, char*date1, char*date2, char*vi)
Psaxnei stin lista tou epipedou 0 tis skip list tous polites tis xoras pou pairnei san orisma pou exoun emvoliastei. Analoga me to orisma me to orisma pou pernei stis imerominies afxanei ton metriti kathe 
periptosi

#int check_population_by_age(Lv_Node* first, char* country, char*date1, char*date2,int* age,char* 
vi)
Psaxnei stin lista tou epipedou 0 stin skip list tous polites tis xoras pou pernei san orisma pou exoun amvoliastei.  Analofa me to orisma pou pernei stis imerominies kai tin ilikia tou politi afxanei tous 
metrites ston pinaka age.
#int insert_new_citizen(Lv_Node** first, Lv_Node** last, char* id,int lv_size,data_node* data,char*vi)  
Arxika afti I sinartisi psaxnei apo to telefteo mexri to proto epipedo na vrei tin thesi tou kenourgiou komvou gia tin periprosi pou tha xreiastei eisagogi kai apothikevei se 2 pinakes dikres pros tous komvous pou vriskontai dexia kai aristera.. Sti sinexeia me tin voithia ton dio pinakon ginetai eisagogi tou neo komvou sto epipedo 0 kai tixaia apofasizetai an tha ginei eidagogi sto epomeno kaianaloga me 
tin periptosi topothetei ton neo komvo stin arxi, sto telos  I anamesa se dio allous
#int delete_from_not_vaccinated(Lv_Node** first, Lv_Node** last, char* id,int lv_size)
Arxika afti I sinartisi psaxnei apo to telefteo mexri to proto epipedo na vrei tin thesi tou komvou gia tin periptosi pou tha xriastei diagrafi kai apothikevei se 2 pinakes deimtes pros tous komvous pou vriskontai dexia kai aristera. Stin sinexia me tin voithia tin dio pinakon ginete diagrafi tou komvou apo to telefteo epipedo pou vriskete mexri to proto analoga me tin periptosi


vaccineMonitor.c
Gia tin ilopoiisi tou monitor xrisimopoiisa akomi mia lista pou periexei ola ta stoixeia ton citizens simperilamvanomenou mias listas pou periexei tous ious kai tis imerominies an exou emvoliastei kai mia lisra pou periexei oles tis xores xexorista.

#data_node* push_data(data_node** first,data_node** last ,char* citizenID, char* firstName, 
char* lastName, char* country, char* age, char* virusName, char* YES_OR_NO, char* dateVaccinated ) 
Kanei eisagogi ton stoixion enos citizen I an iparxei idi eisagei to neo io sta stoixeia tokai epistrefei deikti ston komvo
#int push_country(country_node** first,country_node** last ,char* country)
Eisagogi xoras
#int Search_for_duplicates_data( data_node* first,char* id,  char* virus_n, char* f_name, char* l_name, char* country)
Elegxei an iparxei idi I eggrafi I an iparxei to id me to diaforetika stoixeia
#int Search_for_country( country_node* first,char* country)
Psaxnei gia tin xora kai epistrefei 1 an iparxei kai 0 an den iparxei
#int main(int argc, char** argv)
Arxika arxikopio tis diafores domes pou periegrapsa pio pano gia tis opoies kratao deiktes sto telos kai stin arxi tis listas.Stin sinexeia diavazo grammi grammi apo to arxeio pou dinoume san orisma, xorizo ta stoixeia ton eggrafon, vgazo ta katallila errores kai prosthero stoixeia stis domes opou xriazete. Meta apo tin eisagogi olon ton citizens tou arxeiou sto proto epipedo ton skip lists gia tous vaccinated kai not vavvinated kalo tis sinartiseis pou tha xtisoun ta epomena epipeda. Epita  akolouthei While to opoio termatizei apodesmevontas oles tis domes otan o xristis dosei orisma apo to pliktrologio /exit
O xristis exei tis akolouthes epiloges oti dipote allo epistredei minima lathous kai zitaei nea eisodo:

/vaccineStatusBloom
Vriskei to katallilo bloom filter kai epistrefei ta katallila minimata
/vaccineStatus
Vriskei analofa me ta orismata tin katallili vaccinated skip list kai epistrefei ta katallila minimata.

/populationStatus
Analoga me ta orismata kalei tin check_population pou epistrefei ton aritho ton emvoliasmenon kai tin count_people pou epistrefei to plithos ton citizens apo kapoia xora kai ektiponei to pososto.
/popStatusByAge
Analofa me ta orismata kalei tin check_population_by_age pou epistrefei ton arithmo ton emvoliasmenon gia kathe ilikia kai tin count_people pou epistredei to plithos ton citixens apo kapoia xora kai ektiponei ta pososta.
/insertCitizenRecord
Pairnei ta stoixeia tis neas eggrafis kai ta prosthetei stis diafores domes opou xreiazetai meso tis Change_bloom_filter kai tis insert_new_citizen enimeronei tin skip list kai to bloom filter kai an xriastei me tin delete_from_not_vaccinated diagrafei tin eggrafi.
/vaccinateNow
Pairnei ta stoixeia tis neas eggrafis kai ta prosthetei stis diafores domes opou xriazetai dinontas san orisma tin trexousa imerominia.Meso tis Change_bloom_filter kai tis insert_new_citizen enimeronei tin skip list kai to bloom filter kai an xreiastei me tin delete_from_not_vaccinated diagrafei tin eggrafi.
/list-nonVaccinated-Persons
Tiponei ta dedomena ton non vaccinated Person gia kapoio sigkekrimeno io

Ektelesi meso make file

make 
./vaccineMonitor -c inputFile.txt -b 100 

#1o orisma opoiodipote arxeio .txt (to bash script dimiourgei to  inputFile.txt )
#2ο orisma einai to  sizetou bloom filter

BASH SCRIPT

./testFile.sh virusesFile.txt countriesFile.txt 500 1

(gia kapoio logo sta linux tis sxolis m vgazei permission denied extos an doso prota tin 
entoli chmod +rx testFile.sh     h   bash testFile.sh virusesFile.txt countriesFile.txt 500 1)
