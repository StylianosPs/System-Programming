executable1 = travelMonitor
executable2 = monitor

all : travelMonitor monitor

travelMonitor : travelMonitor.c tmonitor_bloom.c bloom.c
	gcc -o travelMonitor travelMonitor.c tmonitor_bloom.c bloom.c -g

monitor: monitor.c data.c bloom.c
	gcc -o monitor monitor.c data.c bloom.c -g
clean:
	rm -f $(executable1) $(executable2) 