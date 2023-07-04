Stylianos Psara sdi1800226

tmonitor_bloom.c

Gia tin ilopiisi tou travelmonitor xriastika mia lista monitors_bloom pou periexei ton arithmo tou monitor kai dio listees mia me oles tis xores pou asxolite to kathe monitoe kai mia me ta bloom filters gia kathe io.Episis xriastika kai mia lista gia ta requests I opia periezei tin xora stin opia thelei na taxidepsi enas politis kai mia lista me tin imerominia, ton io kata tou opiou tha prepi na exi emvoliasti kai an I etisi tou egkrithike I oxi.


#int push_country(monitors_bloom** first,monitors_bloom** last ,int mon,char* country)
Eisagogi ton xoron pou antistixoun se kathe monitor

#int push_bloom(monitors_bloom** first,monitors_bloom** last,int mon ,char* virus ,char* bloom)
Eisagogi ton bloom filters gia kathe monitor

#int find_monitor(monitors_bloom* first, char* country)
Vriskei to monitor me vasi tin xora

#int search_blooms(monitors_bloom* first,char* countryfrom, char* virus, int hash_array[],int num_of_hash)
Elegxei an enas citizen den exei emvoliasti I isos exi emvoliasti  xrisimopiontas ta bloom filters

#int compare_dates(char* d1,char* d2)
Elegxei gia to orio ton 6 minon anamesa stin imerominia emvoliou kai stin kai stin imerominia pou thelei na taxidepsei o politis

#int compare_dates_for_status(char* d,char* d1,char* d2)
Elegxei an mia imerominia einai anamesa se dio imerominies

#int travelStatus(request_node* first, char* country,char* virus, char* date1, char* date2)
Psaxneis kai metra stin lista ton request gia kathe xora I gia mia xora sigkekrimena ta etimata pou egkrithikan kai aporifthikan kai tiponei ta katallila minimata

#int update_bloom(monitors_bloom** first,monitors_bloom** last,int mon ,char* virus ,char* bloom)
Kanei update ta bloom filters

TravelMonitor.c

#int main(int argc, char** argv)

Arxika arxikopio tis diafores domes pou periegrapsa pio pano gia tis opoies kratao deiktes sto telos kai stin arxi tis listas. Sti sinexia dimiourgo 2 pipes gia kathe monitor.Epita anigo to directory pou dinei o xristis kai dimourgo ena pinaka me onomata xoron kai grafo se kathe monitor me tin methodo round robin metis xores me tis opies tha prepei na asxolithi (gia to write grafo sto pipe lexis megethous mexbuf  kai otan teliosi to string dino ton xaraktira #)  meta perimeno ta monitors na m stiloun ola ta blooms kai gemizo tin lista monitors_bloom.



Stin sinexia se ena While perimeno mia apo tis parakato entoles:

#Stin periptosi tou SIGCHLD vrisko me to waitpid to id tou pediou pou skotothike kai antikathisto ola ta stoixeia tou opos to id tou kai ta pipes me kenourgia 

/travelRequest
Arxika elegxei to katallilo bloom filter kai an tou epistrepsei maybe vriski to monitor sto opio vriskete o sigkekrimenos citizen tou dinei tin entoli kai perimenei an tou epistrepsei no I yes analogos an o politis exi emvoliasti. Otan patrei tin apantisi apo to monitor an o politis exi emvoliastei elegxei an I imerominia einai sto orio ton 6 minon kai vgazei to katallilo minima


/travelStats
Kalei tin travelstatus kai ektiponei ta katallila minimata stin periptosi pou den dosi xora o xristis dino to -1.

/addVaccinationRecords
Stelnei SIQUSR1 sto katallilo monitor kai perimenei na parei ta updates ton monitors.

/searchVaccinationStatus
Stelnei tin entoli se kathe monitor kai perimenei tin apantisi gia na tin ektiposi

/exit 
Kanei kill ola ta moniros kai apodesmevei oles tis domes



bloom.c (to pira apo tin proti ergasia)

Gia tin domi tou bloom filter xrisimopio mia lista h opoia periexei to onoma tou iou kai ena deikth ston opoio desmevo mnhmh gia to bloom filter

#int push(bloom_node** first,bloom_node** last ,char* virus, int bloomSize )
Dimiourgei ena komvo bloom_node arxikopoiei to bloom filter kai ton prosthetei sto telos ths listas

#int Search_for_virus_bloom( bloom_node* first, char* virus)
Psaxnei an iparxei komvos gia ton io pou pairnei san orisma kai an iparxei epistrefei 1 alios epistrefei 0

#int Change_bloom_filter(bloom_node* first, char* virus, int hash_array[], int num_of_hash)
Vriskei ton komvo tou iou pou pairnei san orisma kai anathetei thn timh 1 stis theseis tou pinaka pou pairnei apo ton hash_array
#int check_bloom(bloom_node* first, char* virus, int hash_array[],int num_of_hash)
Elegxei an oi theseis tou bloom filter pou pairnei apo to hash_array periexoun ena toulaxisto 0 kai tiponei to minima NOT VACCINATED alios an einai ola 1 tiponei MAYBE

data.c

Gia tin ilopoiisi tou monitor xrisimopoiisa akomi mia lista pou periexei ola ta stoixeia ton citizens simperilamvanomenou mias listas pou periexei tous ious kai tis imerominies an exou emvoliastei kai mia lisra pou periexei oles tis xores xexorista.Episis exo mia lista me ta arxeia txt pou analamvanei kathe monitor.

#data_node* push_data(data_node** first,data_node** last ,char* citizenID, char* firstName, char* lastName, char* country, char* age, char* virusName, char* YES_OR_NO, char* dateVaccinated ) 
Kanei eisagogi ton stoixion enos citizen I an iparxei idi eisagei to neo io sta stoixeia tokai epistrefei deikti ston komvo

#int Search_for_duplicates_data( data_node* first,char* id,  char* virus_n, char* f_name, char* l_name, char* country)
Elegxei an iparxei idi I eggrafi I an iparxei to id me to diaforetika stoixeia

#int travelRequest(data_node* first, char* id, char* v_name,char** ans, int* accepted, int* rejected )
Psaxnei stin lista me ta records kai epistrefei an o citizen me to trexon id exei emvoliastei I oxi.

#int push_country_files(country_files** first, country_files** last, char* file)
Kanei isagogi ena onoma rxeioutxt stin lista.

#int SearchVaccinationStatus(data_node* first, char* ID, char** output )
Psaxnei gia to trexon id gia pious ious exei emvoliastei kai gia pious oxi.int

#logfile(int accepted, int rejected, char* countries)
dimiourgei to log file tou monitor.

Monitor.c

#int main(int argc, char** argv)

Arxika arxikopio tis diafores domes pou periegrapsa pio pano gia tis opoies kratao deiktes sto telos kai stin arxi tis listas. Stin sinexeia anigo ta pipes kai perimeno na dievasei stoixeia pou einai aparetita gia tin sinexia opos to bloomSize, to megisto megethos pou mporei exei to buffer, to directory kai tis xores me tis opoies tha asxolithei to trexon monitor. Sti sinexia diavazo ta records apo ola ta txt , xorizo ta stoixeia ton eggrafon, vgazo ta katallila errors kai dimiourgo ta bloom filters ta opoia stelno meso ton pipes sto travelmonitor . Epita  akolouthei While sto opoio perimenei perimenei to monitor entoli

Arxika exo elegxo kai se periptosi SIGINT SIGQUIT  dimioutgo to logfile kai sinexizo allio dievazo apo to pipe to request pou stelnei o travel monitor ektos kai an parei SIGUSR1 pou dilonei oti o xristis exei prosthesei kenourgio arxeio se kapia apo tis xores

To monitor mporei na kanei ta akoloutha pragmata  analoga me to ti epilexei o xristis:

/travelRequest
Psaxnei to citizen me to id pou edose o xristis grafei sto travel monitor yes I no analoga me to an o citizen exi emvoliasti I oxi

/addVaccinationRecords
Psaxnei ta  directories giakathe xora pou diaxirizete kai an vrei arxeia txt pou den exei kanei elegxo tote kanei update ta bloom filters kai ta stelnei piso sto travel monitor

/searchVaccinationStatus
Kali tis katalliles sinartissi kai stelnei meso pipes sto travel monitor to katallilo output gia to trexon id



Elegxos
O elegxos ton signals egine topothetontas se diafora simia tou programmatos tis entoles kill(pid_array[j],SIGKILL) kill(pid_array[j],SIGQUIT) kill(pid_arrat[j],SIGINT)


Ektelesi meso make file

make 
./travelMonitor -m 3 -b 5 -s 10 -i input_dirr


BASH SCRIPT
./create_infiles.sh inputfile input_dir 3

(gia kapoio logo sta linux tis sxolis m vgazei permission denied extos an doso prota tin 
entoli chmod +rx create_inputfiles.sh     h   bash create_infiles.sh inputfile input_dir 3 )