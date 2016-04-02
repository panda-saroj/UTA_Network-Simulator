/*
 * output_threads.h
 *
 *  Created on: Feb 27, 2016
 *      Author: saroj
 */

#ifndef INPUT_THREADS_H_
#define INPUT_THREADS_H_

#include <semaphore.h>
#include "cn2simulator.h"
#include "queue.h"

extern int output_thread1_slp_time;
extern int output_thread2_slp_time ;
extern int output_thread3_slp_time ;


void *ipv4_output_queue_thread_function( void *ptr );

void process_ipv4_port1_packets(ipv4_port_param *ptr);
void process_ipv4_port2_packets(ipv4_port_param *ptr);
void process_ipv4_port3_packets(ipv4_port_param *ptr);


void initialize_sem_mutex();
void initialize_output_files();

void write_Packets_To_Files();

int start_output_link_threads();


#endif /* INPUT_THREADS_H_ */
