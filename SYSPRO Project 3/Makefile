executable1 = travelMonitorClient
executable2 = monitorServer

all : travelMonitorClient monitorServer

travelMonitorClient : travelMonitorClient.c tmonitor_bloom.c bloom.c 
	gcc -o travelMonitorClient travelMonitorClient.c tmonitor_bloom.c bloom.c -g

monitorServer: monitorServer.c data.c bloom.c threads.c
	gcc -o monitorServer monitorServer.c data.c bloom.c threads.c -lpthread -g
clean:
	rm -f $(executable1) $(executable2) 