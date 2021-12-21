CC=gcc
FLAGS=-Wall -g
.PHONY: all clean
all: measure sender
Measure.o:Measure.c
	$(CC) $(FLAGS) -c Measure.c
sender.o:sender.c
	$(CC) $(FLAGS) -c sender.c
measure: Measure.o
	$(CC) $(FLAGS) Measure.o -o measure
sender: sender.o
	$(CC) $(FLAGS) sender.o -o sender

clean:
	rm  *.o measure sender
