CC=gcc 
CFLAGS=-Wall

all: cn2Simulator

#cn2Simulator: read_input_file.o input_threads.o queue.o bld_routing_tbl.o output_threads.o cn2simulator.o
#	gcc -pthread -o cn2Simulator  read_input_file.o input_threads.o queue.o bld_routing_tbl.o output_threads.o cn2simulator.o

cn2Simulator: queue.o bld_routing_tbl.o output_threads.o cn2simulator.o
#	gcc -pthread -o cn2Simulator queue.o bld_routing_tbl.o output_threads.o cn2simulator.o
	g++ -pthread -o cn2Simulator queue.o bld_routing_tbl.o output_threads.o cn2simulator.o

#read_input_file.o: read_input_file.c read_input_file.h
#	gcc -c  read_input_file.c read_input_file.h


#input_threads.o: input_threads.c input_threads.h
#	gcc -c input_threads.c input_threads.h

queue.o: queue.c queue.h
	gcc -c  queue.c queue.h
#	g++ -c  queue.c queue.h

bld_routing_tbl.o: bld_routing_tbl.c bld_routing_tbl.h
	gcc -c  bld_routing_tbl.c bld_routing_tbl.h
#	g++ -c  bld_routing_tbl.c bld_routing_tbl.h
	
output_threads.o: output_threads.c output_threads.h
	gcc -c  output_threads.c output_threads.h
#	g++ -c  output_threads.c output_threads.h

cn2simulator.o: cn2simulator.c cn2simulator.h
	gcc -c  cn2simulator.c cn2simulator.h
#	g++ -c  cn2simulator.c cn2simulator.h
	
clean:
	rm -rf cn2Simulator *.o *.gch outfile* log.txt table