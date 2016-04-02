/*
 * Output_threads.c
 *
 *  Created on: Feb 27, 2016
 *      Author: saroj
 */

#ifndef INPUT_THREADS_C_
#define INPUT_THREADS_C_


#include "output_threads.h"
#include "cn2simulator.h"

//Variable to Keep track of Packets processed on each output port
int port1_pkt_count = 0, port2_pkt_count = 0, port3_pkt_count = 0;

//Variable to Keep track of Packets processed from each output queue
int port1_q1_pkt_count = 0, port1_q2_pkt_count = 0;
int port2_q1_pkt_count = 0, port2_q2_pkt_count = 0;
int port3_q1_pkt_count = 0, port3_q2_pkt_count = 0;

//Variables to keep track of Packets added to each queue
extern int port1_q1_pkt_counter;
extern int port1_q2_pkt_counter;
extern int port2_q1_pkt_counter;
extern int port2_q2_pkt_counter;
extern int port3_q1_pkt_counter;
extern int port3_q2_pkt_counter;

//Output Queue Link Thread Parameters
extern ipv4_port_param ipv4_port1_param;
extern ipv4_port_param ipv4_port2_param;
extern ipv4_port_param ipv4_port3_param;

//Semaphore Names for each IPV4 Queue
sem_t ipv4_port1_sem, ipv4_port2_sem, ipv4_port3_sem;

//VAriables to Hold Current Max Residence time of each Queue
int port1_q1_max_res_tm = 0, port1_q2_max_res_tm = 0;
int port2_q1_max_res_tm = 0, port2_q2_max_res_tm = 0;
int port3_q1_max_res_tm = 0, port3_q2_max_res_tm = 0;


//Mutexes For each IPV4 Queue

pthread_mutex_t ipv4_port1_queue1_mutex;
pthread_mutex_t ipv4_port1_queue2_mutex;

pthread_mutex_t ipv4_port2_queue1_mutex;
pthread_mutex_t ipv4_port2_queue2_mutex;

pthread_mutex_t ipv4_port3_queue1_mutex;
pthread_mutex_t ipv4_port3_queue2_mutex;


 pthread_t output_thread1;
 pthread_t output_thread2;
 pthread_t output_thread3;

extern FILE *outfile1;
extern FILE *outfile2;
extern FILE *outfile3;

struct timespec out_q1_1st_pkt_depart_time;
struct timespec out_q2_1st_pkt_depart_time;
struct timespec out_q3_1st_pkt_depart_time;

ipv4Node *Port1_q1_ipv4_Output_Buffer_Head = NULL;
ipv4Node *Port1_q1_ipv4_Output_Buffer_Rear = NULL;

ipv4Node *Port1_q2_ipv4_Output_Buffer_Head = NULL;
ipv4Node *Port1_q2_ipv4_Output_Buffer_Rear = NULL;

ipv4Node *Port2_q1_ipv4_Output_Buffer_Head = NULL;
ipv4Node *Port2_q1_ipv4_Output_Buffer_Rear = NULL;

ipv4Node *Port2_q2_ipv4_Output_Buffer_Head = NULL;
ipv4Node *Port2_q2_ipv4_Output_Buffer_Rear = NULL;

ipv4Node *Port3_q1_ipv4_Output_Buffer_Head = NULL;
ipv4Node *Port3_q1_ipv4_Output_Buffer_Rear = NULL;

ipv4Node *Port3_q2_ipv4_Output_Buffer_Head = NULL;
ipv4Node *Port3_q2_ipv4_Output_Buffer_Rear = NULL;


int port1_q1_total_res_time =0, port1_q2_total_res_time = 0;
int port2_q1_total_res_time =0, port2_q2_total_res_time = 0;
int port3_q1_total_res_time =0, port3_q2_total_res_time = 0;



void addTo_IPV4_Output_Buffer(ipv4_port_no port_no, ipv4_queue_no q_no, ipv4Packet *ipv4Pkt)
{
	ipv4Node *node = (ipv4Node *)malloc(sizeof(ipv4Node));
	node->ipv4Pkt = ipv4Pkt;
	node->next = NULL;

	switch(port_no)
		{
		case IPV4_PORT_1:
			if (IPV4_QUEUE_1 == q_no)
			{

				if(Port1_q1_ipv4_Output_Buffer_Head == NULL && Port1_q1_ipv4_Output_Buffer_Rear == NULL)
				{
					Port1_q1_ipv4_Output_Buffer_Head = Port1_q1_ipv4_Output_Buffer_Rear = node;
				}
				else
				{

					Port1_q1_ipv4_Output_Buffer_Rear->next = node;
					Port1_q1_ipv4_Output_Buffer_Rear = node;
				}

			}
			else if (IPV4_QUEUE_2 == q_no)
			{

				//Increment the Packet counter
				port1_q2_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port1_q2_pkt_counter;

				if(Port1_q2_ipv4_Output_Buffer_Head == NULL && Port1_q2_ipv4_Output_Buffer_Rear == NULL)
				{
					Port1_q2_ipv4_Output_Buffer_Head = Port1_q2_ipv4_Output_Buffer_Rear = node;
				}
				else
				{

					Port1_q2_ipv4_Output_Buffer_Rear->next = node;
					Port1_q2_ipv4_Output_Buffer_Rear = node;
				}
			}

			break;

		case IPV4_PORT_2:
			if (IPV4_QUEUE_1 == q_no)
			{
				//Increment the Packet counter
				port2_q1_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port2_q1_pkt_counter;

				if(Port2_q1_ipv4_Output_Buffer_Head == NULL && Port2_q1_ipv4_Output_Buffer_Rear == NULL)
				{
					Port2_q1_ipv4_Output_Buffer_Head = Port2_q1_ipv4_Output_Buffer_Rear = node;
				}
				else
				{

					Port2_q1_ipv4_Output_Buffer_Rear->next = node;
					Port2_q1_ipv4_Output_Buffer_Rear = node;
				}

			}
			else if (IPV4_QUEUE_2 == q_no)
			{
				//Increment the Packet counter
				port2_q2_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port2_q2_pkt_counter;

				if(Port2_q2_ipv4_Output_Buffer_Head == NULL && Port2_q2_ipv4_Output_Buffer_Rear == NULL)
				{
					Port2_q2_ipv4_Output_Buffer_Head = Port2_q2_ipv4_Output_Buffer_Rear = node;
				}
				else
				{

					Port2_q2_ipv4_Output_Buffer_Rear->next = node;
					Port2_q2_ipv4_Output_Buffer_Rear = node;
				}
			}

			break;

		case IPV4_PORT_3:
			if (IPV4_QUEUE_1 == q_no)
			{
				//Increment the Packet counter
				port3_q1_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port3_q1_pkt_counter;

				if(Port3_q1_ipv4_Output_Buffer_Head == NULL && Port3_q1_ipv4_Output_Buffer_Rear == NULL)
				{
					Port3_q1_ipv4_Output_Buffer_Head = Port3_q1_ipv4_Output_Buffer_Rear = node;
				}
				else
				{

					Port3_q1_ipv4_Output_Buffer_Rear->next = node;
					Port3_q1_ipv4_Output_Buffer_Rear = node;
				}

			}
			else if (IPV4_QUEUE_2 == q_no)
			{
				//Increment the Packet counter
				port3_q2_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port3_q2_pkt_counter;

				if(Port3_q2_ipv4_Output_Buffer_Head == NULL && Port3_q2_ipv4_Output_Buffer_Rear == NULL)
				{
					Port3_q2_ipv4_Output_Buffer_Head = Port3_q2_ipv4_Output_Buffer_Rear = node;
				}
				else
				{

					Port3_q2_ipv4_Output_Buffer_Rear->next = node;
					Port3_q2_ipv4_Output_Buffer_Rear = node;
				}
			}

			break;
		}

}

void initialize_sem_mutex()
{
//Mutexes for Port1 Queues
	pthread_mutex_init(&ipv4_port1_queue1_mutex, NULL);
	pthread_mutex_init(&ipv4_port1_queue2_mutex, NULL);

//Mutexes for Port2 Queues
	pthread_mutex_init(&ipv4_port2_queue1_mutex, NULL);
	pthread_mutex_init(&ipv4_port2_queue2_mutex, NULL);

	//Mutexes for Port3 Queues
	pthread_mutex_init(&ipv4_port3_queue1_mutex, NULL);
	pthread_mutex_init(&ipv4_port3_queue2_mutex, NULL);

//Initialize the semaphores of each  port
	sem_init(&ipv4_port1_sem, 0, 0);
	sem_init(&ipv4_port2_sem, 0, 0);
	sem_init(&ipv4_port3_sem, 0, 0);
		
}



/*This function calculates the relative speed of each link based on rate
 * limit for both input and output links. This then transfers that to nano
 * second sleeping for each thread
 * each link
 */


void *ipv4_output_queue_thread_function( void *ptr )
{
	int datetime_diff_ms;
	ipv4_port_param *pParam;

	struct timeval t0, t1;

	pParam = (ipv4_port_param *)ptr;

//	printf("Thread For Port %d\n", pParam->port_no);
//	gettimeofday(&t0, NULL);

/*
 * To simulate speed limit, make the thread to sleep for the calculated time
 */
//	usleep(qp->slp_time);

	switch( pParam->port_no )
	{
		case IPV4_PORT_1:
//			printf("Output Thread 1 had slept for :%d\n", qparam->slp_time);
			process_ipv4_port1_packets(pParam);
			break;

		case IPV4_PORT_2:
//			printf("Output Thread 2 had slept for :%d\n", qparam->slp_time);
			process_ipv4_port2_packets(pParam);
			break;

		case IPV4_PORT_3:
//			printf("Output Thread 3 had slept for :%d\n", qparam->slp_time);
			process_ipv4_port3_packets(pParam);
			break;

		default:
			printf("%s %d It should never come here \n", __FILE__, __LINE__);
			exit(0);


	}

	return ptr;
}

int calculate_sleep_time(int pkt_len, int speed, int time_spent)
{
	int send_time =0;
	int sleep_time =0;
	send_time = ((pkt_len * 8)/speed) * SECOND_TO_NANOSEC;

	sleep_time = send_time - time_spent; //Return sleep_time in nano second
	return sleep_time;

}

void process_ipv4_port1_packets(ipv4_port_param *ptr)
{

	struct timespec start_time, end_time;
	int pkt_len = 0;
	int processing_time = 0;
	int sleep_time = 0;
	int thread_sleep_time = 0;


	ipv4Packet *q1_ipv4Pkt = NULL; //Holds Packet from q1 port 1
	ipv4Packet *q2_ipv4Pkt = NULL; //Holds Packet from q2 port 1

	ipv4_port_param *pParam = ptr;

	int q1_current_res_time; //Holds Q1 Current Packet Residence Time
	int q2_current_res_time; //Holds Q2 Current Packet Residence Time
	
	while(TRUE)
	{

	
	//Wait For Packets To Arrive In one of the the Queue To be Processed
//		printf("Waiting on ipv4_port1_sem \n");
		sem_wait(&ipv4_port1_sem);

		//Take Time stamp
		clock_gettime(CLOCK_REALTIME, &start_time);
//		pkt_len = 0;

	
	//Got Signal, Now Lock The queue one by one and process packets from them
		pthread_mutex_lock (&ipv4_port1_queue1_mutex);
		q1_ipv4Pkt = (ipv4Packet *)ipv4Dequeue(IPV4_PORT_1, IPV4_QUEUE_1);
		pthread_mutex_unlock(&ipv4_port1_queue1_mutex);
		
		if (NULL != q1_ipv4Pkt)
		{
//			pkt_len += q1_ipv4Pkt->pkt_len;

			port1_pkt_count++; //Count of Packets Handled on Output Port 1
			port1_q1_pkt_count++; //Count of Packets Handled on Output Port 1 Queue 1

			//Found Packet in q1, Process it
			fwrite(q1_ipv4Pkt->complete_packet, 1, q1_ipv4Pkt->pkt_len, outfile1);

//			addTo_IPV4_Output_Buffer(IPV4_PORT_1, IPV4_QUEUE_1, q1_ipv4Pkt);

			//Take Time stamp To find, Actual Time Taken To Process The Packet
			clock_gettime(CLOCK_REALTIME, &end_time);

			//Processing Time In Writing To File
			processing_time = end_time.tv_nsec - start_time.tv_nsec;

			//Now Calculate How much Time we need to sleep
			sleep_time = calculate_sleep_time(q1_ipv4Pkt->pkt_len, pParam->speed, processing_time);

			//Returned sleep_time is in nano second, convert that to micro second
			thread_sleep_time = sleep_time/MICROSEC_TO_NANOSEC;

			//Sleep to simulate actual time it would have taken with the router
			if (thread_sleep_time > 0)
				usleep(thread_sleep_time);

			//Take Departure Time stamp. Not using it now
			clock_gettime(CLOCK_REALTIME, &(q1_ipv4Pkt->depart_time));

			/*
			 * Calculate Different Times
			 */
			q1_ipv4Pkt->wait_time = start_time.tv_nsec - q1_ipv4Pkt->arrival_time.tv_nsec;

			q1_current_res_time =  q1_ipv4Pkt->wait_time + processing_time + sleep_time;
			q1_ipv4Pkt->res_time = q1_current_res_time;

			//Add The Residence Time to the total Res Time
			port1_q1_total_res_time += q1_current_res_time;

			if (port1_q1_max_res_tm < q1_current_res_time)
			{
				//Update the max residence time
				port1_q1_max_res_tm = q1_current_res_time;
			}

	//		usleep(qparam->slp_time);

		}

		//Now Take out Packet from The other Queue

		pthread_mutex_lock (&ipv4_port1_queue2_mutex);
		q2_ipv4Pkt = (ipv4Packet *)ipv4Dequeue(IPV4_PORT_1, IPV4_QUEUE_2);

	//Unlock The Queue
		pthread_mutex_unlock(&ipv4_port1_queue2_mutex);

		if (NULL != q2_ipv4Pkt)
		{

			port1_pkt_count++;
			port1_q2_pkt_count++; //Count of Packets Handled on Output Port 1 Queue 2
			//Found Packet in q1, Process it
			fwrite(q2_ipv4Pkt->complete_packet, 1, q2_ipv4Pkt->pkt_len, outfile1);

//			addTo_IPV4_Output_Buffer(IPV4_PORT_1, IPV4_QUEUE_2, q2_ipv4Pkt);


			//Take Time stamp
			clock_gettime(CLOCK_REALTIME, &end_time);
			processing_time = end_time.tv_nsec - start_time.tv_nsec;

			//Now Calculate How much Time we need to sleep
			sleep_time = calculate_sleep_time(q2_ipv4Pkt->pkt_len, pParam->speed, processing_time);

			//Returned sleep_time is in nano second, convert that to micro second
			thread_sleep_time = sleep_time/MICROSEC_TO_NANOSEC;

			if (thread_sleep_time > 0)
				usleep(thread_sleep_time);


			//Take Departure Time stamp
			clock_gettime(CLOCK_REALTIME, &(q2_ipv4Pkt->depart_time));

			/*
			 * Calculate Different Times
			 */
			q2_ipv4Pkt->wait_time = start_time.tv_nsec - q2_ipv4Pkt->arrival_time.tv_nsec;

			q2_current_res_time =  q2_ipv4Pkt->wait_time + processing_time + sleep_time;
			q2_ipv4Pkt->res_time = q2_current_res_time;

			//Add The Residence Time to the total Res Time
			port1_q2_total_res_time += q2_current_res_time;

			if (port1_q2_max_res_tm < q2_current_res_time)
			{
				//Update the max residence time
				port1_q2_max_res_tm = q2_current_res_time;
			}

		}


		if ((port1_q1_pkt_count == port1_q1_pkt_counter) && (port1_q2_pkt_count == port1_q2_pkt_counter))
		{
			printf("Processed All Packets from both Queues on Port 1\n");
			break;
		}
			
	}
	printf("Total Packet Processed in output_thread 1 :%d %s %d\n", port1_pkt_count, __FILE__, __LINE__);
	fclose(outfile1);
	return;
}

void process_ipv4_port2_packets(ipv4_port_param *ptr)
{
	struct timespec start_time, end_time;
	int pkt_len = 0;
	int processing_time = 0;
	int sleep_time = 0;
	int thread_sleep_time = 0;

	ipv4Packet *q1_ipv4Pkt = NULL; //Holds Packet from q1 port 1
	ipv4Packet *q2_ipv4Pkt = NULL; //Holds Packet from q2 port 1

	ipv4_port_param *pParam = ptr;

	int q1_current_res_time; //Holds Q1 Current Packet Residence Time
	int q2_current_res_time; //Holds Q1 Current Packet Residence Time
	
	while(TRUE)
	{

	
	//Wait For Packets To Arrive In one of the the Queue To be Processed
//		printf("Waiting on ipv4_port2_sem \n");
		sem_wait(&ipv4_port2_sem);

		//Take Time stamp
		clock_gettime(CLOCK_REALTIME, &start_time);
		pkt_len = 0;
	
	//Got Signal, Now Lock The queue one by one and process packet one by one
		pthread_mutex_lock (&ipv4_port2_queue1_mutex);
		q1_ipv4Pkt = (ipv4Packet *)ipv4Dequeue(IPV4_PORT_2, IPV4_QUEUE_1);
		pthread_mutex_unlock(&ipv4_port2_queue1_mutex);
		
		if (NULL != q1_ipv4Pkt)
		{
			pkt_len += q1_ipv4Pkt->pkt_len;


			port2_pkt_count++;
			port2_q1_pkt_count++; //Count of Packets Handled on Output Port 2 Queue 1

			//Found Packet in q1, Process it
			fwrite(q1_ipv4Pkt->complete_packet, 1, q1_ipv4Pkt->pkt_len, outfile2);

//			addTo_IPV4_Output_Buffer(IPV4_PORT_2, IPV4_QUEUE_1, q1_ipv4Pkt);

			//Take Time stamp To find, Actual Time Taken To Process The Packet
			clock_gettime(CLOCK_REALTIME, &end_time);

			//Processing Time In Writing To File
			processing_time = end_time.tv_nsec - start_time.tv_nsec;

			//Now Calculate How much Time we need to sleep
			sleep_time = calculate_sleep_time(q1_ipv4Pkt->pkt_len, pParam->speed, processing_time);

			//Returned sleep_time is in nano second, convert that to micro second
			thread_sleep_time = sleep_time/MICROSEC_TO_NANOSEC;

			//Sleep to simulate actual time it would have taken with the router
			if (thread_sleep_time > 0)
				usleep(thread_sleep_time);

			//Take Departure Time stamp. Not using it now
			clock_gettime(CLOCK_REALTIME, &(q1_ipv4Pkt->depart_time));

			/*
			 * Calculate Different Times
			 */
			q1_ipv4Pkt->wait_time = start_time.tv_nsec - q1_ipv4Pkt->arrival_time.tv_nsec;

			q1_current_res_time =  q1_ipv4Pkt->wait_time + processing_time + sleep_time;
			q1_ipv4Pkt->res_time = q1_current_res_time;


			//Add The Residence Time to the total Res Time
			port2_q1_total_res_time += q1_current_res_time;

			if (port2_q1_max_res_tm < q1_current_res_time)
			{
				//Update the max residence time
				port2_q1_max_res_tm = q1_current_res_time;
			}

		}

		//Now Take out Packet from The other Queue. Simple Round Robin

		pthread_mutex_lock (&ipv4_port2_queue2_mutex);
		q2_ipv4Pkt = (ipv4Packet *)ipv4Dequeue(IPV4_PORT_2, IPV4_QUEUE_2);
		pthread_mutex_unlock(&ipv4_port2_queue2_mutex);

		if (NULL != q2_ipv4Pkt)
		{

			pkt_len += q2_ipv4Pkt->pkt_len;

			port2_pkt_count++;
			port2_q2_pkt_count++; //Count of Packets Handled on Output Port 2 Queue 2

			//Found Packet in q1, Process it
			fwrite(q2_ipv4Pkt->complete_packet, 1, q2_ipv4Pkt->pkt_len, outfile2);

//			addTo_IPV4_Output_Buffer(IPV4_PORT_2, IPV4_QUEUE_2, q2_ipv4Pkt);

			//Take Time stamp
			clock_gettime(CLOCK_REALTIME, &end_time);

			processing_time = end_time.tv_nsec - start_time.tv_nsec;

			//Now Calculate How much Time we need to sleep
			sleep_time = calculate_sleep_time(q2_ipv4Pkt->pkt_len, pParam->speed, processing_time);

			//Returned sleep_time is in nano second, convert that to micro second
			thread_sleep_time = sleep_time/MICROSEC_TO_NANOSEC;

			if (thread_sleep_time > 0)
				usleep(thread_sleep_time);


			//Take Departure Time stamp
			clock_gettime(CLOCK_REALTIME, &(q2_ipv4Pkt->depart_time));

			/*
			 * Calculate Different Times
			 */
			q2_ipv4Pkt->wait_time = start_time.tv_nsec - q2_ipv4Pkt->arrival_time.tv_nsec;

			q2_current_res_time =  q2_ipv4Pkt->wait_time + processing_time + sleep_time;
			q2_ipv4Pkt->res_time = q2_current_res_time;


			//Add The Residence Time to the total Res Time
			port2_q2_total_res_time += q2_current_res_time;

			if (port2_q2_max_res_tm < q2_current_res_time)
			{
				//Update the max residence time
				port2_q2_max_res_tm = q2_current_res_time;
			}


		}


		if ((port2_q1_pkt_count == port2_q1_pkt_counter) && (port2_q2_pkt_count == port2_q2_pkt_counter))
		{
			printf("Processed All Packets from both Queues on Port 2\n");
			break;
		}
			
	}
	printf("Total Packet Processed in output_thread 2 :%d %s %d\n", port2_pkt_count, __FILE__, __LINE__);
	fclose(outfile2);
	return;
}

void process_ipv4_port3_packets(ipv4_port_param *ptr)
{

	struct timespec start_time, end_time;
	int pkt_len = 0;
	int processing_time = 0;
	int sleep_time = 0;
	int thread_sleep_time = 0;


	ipv4Packet *q1_ipv4Pkt = NULL; //Holds Packet from q1 port 1
	ipv4Packet *q2_ipv4Pkt = NULL; //Holds Packet from q2 port 1

	ipv4_port_param *pParam = ptr;

	int q1_current_res_time; //Holds Q1 Current Packet Residence Time
	int q2_current_res_time; //Holds Q1 Current Packet Residence Time
	
	while(TRUE)
	{

	
	//Wait For Packets To Arrive In one of the the Queue To be Processed
//		printf("Waiting on ipv4_port2_sem \n");
		sem_wait(&ipv4_port3_sem);

		//Take Time stamp
		clock_gettime(CLOCK_REALTIME, &start_time);
		pkt_len = 0;
	
	//Got Signal, Now Lock The queue one by one and process packet one by one
		pthread_mutex_lock (&ipv4_port3_queue1_mutex);
		q1_ipv4Pkt = (ipv4Packet *)ipv4Dequeue(IPV4_PORT_3, IPV4_QUEUE_1);
		pthread_mutex_unlock(&ipv4_port3_queue1_mutex);
		
		if (NULL != q1_ipv4Pkt)
		{
			pkt_len += q1_ipv4Pkt->pkt_len;

			port3_pkt_count++;
			port3_q1_pkt_count++; //Count of Packets Handled on Output Port 3 Queue 1

			//Found Packet in q1, Process it
			fwrite(q1_ipv4Pkt->complete_packet, 1, q1_ipv4Pkt->pkt_len, outfile3);

//			addTo_IPV4_Output_Buffer(IPV4_PORT_3, IPV4_QUEUE_1, q1_ipv4Pkt);

			//Take Time stamp To find, Actual Time Taken To Process The Packet
			clock_gettime(CLOCK_REALTIME, &end_time);

			//Processing Time In Writing To File
			processing_time = end_time.tv_nsec - start_time.tv_nsec;

			//Now Calculate How much Time we need to sleep
			sleep_time = calculate_sleep_time(q1_ipv4Pkt->pkt_len, pParam->speed, processing_time);

			//Returned sleep_time is in nano second, convert that to micro second
			thread_sleep_time = sleep_time/MICROSEC_TO_NANOSEC;

			//Sleep to simulate actual time it would have taken with the router
			if (thread_sleep_time > 0)
				usleep(thread_sleep_time);

			//Take Departure Time stamp. Not using it now
			clock_gettime(CLOCK_REALTIME, &(q1_ipv4Pkt->depart_time));

			/*
			 * Calculate Different Times
			 */
			q1_ipv4Pkt->wait_time = start_time.tv_nsec - q1_ipv4Pkt->arrival_time.tv_nsec;

			q1_current_res_time =  q1_ipv4Pkt->wait_time + processing_time + sleep_time;
			q1_ipv4Pkt->res_time = q1_current_res_time;

			//Add The Residence Time to the total Res Time
			port3_q1_total_res_time += q1_current_res_time;

			if (port3_q1_max_res_tm < q1_current_res_time)
			{
				//Update the max residence time
				port3_q1_max_res_tm = q1_current_res_time;
			}

		}

		//Now Take out Packet from The other Queue Q2

		pthread_mutex_lock (&ipv4_port3_queue2_mutex);
		q2_ipv4Pkt = (ipv4Packet *)ipv4Dequeue(IPV4_PORT_3, IPV4_QUEUE_2);
		pthread_mutex_unlock(&ipv4_port3_queue2_mutex);

		if (NULL != q2_ipv4Pkt)
		{
			pkt_len += q2_ipv4Pkt->pkt_len;

			port3_pkt_count++;
			port3_q2_pkt_count++; //Count of Packets Handled on Output Port 3 Queue 2

			//Found Packet in q1, Process it
			fwrite(q2_ipv4Pkt->complete_packet, 1, q2_ipv4Pkt->pkt_len, outfile3);

//			addTo_IPV4_Output_Buffer(IPV4_PORT_3, IPV4_QUEUE_2, q2_ipv4Pkt);

			//Take Time stamp
			clock_gettime(CLOCK_REALTIME, &end_time);

			processing_time = end_time.tv_nsec - start_time.tv_nsec;

			//Now Calculate How much Time we need to sleep
			sleep_time = calculate_sleep_time(q2_ipv4Pkt->pkt_len, pParam->speed, processing_time);

			//Returned sleep_time is in nano second, convert that to micro second
			thread_sleep_time = sleep_time/MICROSEC_TO_NANOSEC;

			if (thread_sleep_time > 0)
				usleep(thread_sleep_time);


			//Take Departure Time stamp
			clock_gettime(CLOCK_REALTIME, &(q2_ipv4Pkt->depart_time));

			/*
			 * Calculate Different Times
			 */
			q2_ipv4Pkt->wait_time = start_time.tv_nsec - q2_ipv4Pkt->arrival_time.tv_nsec;

			q2_current_res_time =  q2_ipv4Pkt->wait_time + processing_time + sleep_time;
			q2_ipv4Pkt->res_time = q2_current_res_time;

			//Add The Residence Time to the total Res Time
			port3_q2_total_res_time += q2_current_res_time;

			if (port3_q2_max_res_tm < q2_current_res_time)
			{
				//Update the max residence time
				port3_q2_max_res_tm = q2_current_res_time;
			}


		}

		if ((port3_q1_pkt_count == port3_q1_pkt_counter) && (port3_q2_pkt_count == port3_q2_pkt_counter))
		{
			printf("Processed All Packets from both Queues on Port 1\n");
			break;
		}

	}
	printf("Total Packet Processed in output_thread 3 :%d %s %d\n", port3_pkt_count, __FILE__, __LINE__);
	fclose(outfile3);
	return;
}


int start_output_link_threads()
{

	 int  iret1, iret2, iret3;

//Set Each Thread Parameters
	 ipv4_port1_param.port_no = IPV4_PORT_1;
	 ipv4_port1_param.slp_time = output_thread1_slp_time;

	 ipv4_port2_param.port_no = IPV4_PORT_2;
	 ipv4_port2_param.slp_time = output_thread2_slp_time;

	 ipv4_port3_param.port_no = IPV4_PORT_3;
	 ipv4_port3_param.slp_time = output_thread2_slp_time;



//	 calculate_rate_limit();

	 iret1 = pthread_create( &output_thread1, NULL, ipv4_output_queue_thread_function, (void *)&ipv4_port1_param );
	 if (iret1)
	 {
		 printf("Error - pthread_create() output_thread1 return code: %d\n",iret1);
		 return FALSE;
	 }

	 iret2 = pthread_create( &output_thread2, NULL, ipv4_output_queue_thread_function, (void *)&ipv4_port2_param);
	 	 if (iret2)
	 	 {
	 		 printf("Error - pthread_create() output_thread2 return code: %d\n",iret2);
	 		 return FALSE;
	 	 }

		 iret3 = pthread_create( &output_thread3, NULL, ipv4_output_queue_thread_function, (void *)&ipv4_port3_param );
		 if (iret3)
		 {
			 printf("Error - pthread_create() output_thread3 return code: %d\n",iret3);
			 return FALSE;
		 }

		 return TRUE;
}

void write_Packets_To_Files()
{

	ipv4Node *Port1_q1_ipv4_Output_Buffer_Head = NULL;
	ipv4Node *Port1_q1_ipv4_Output_Buffer_Rear = NULL;

	ipv4Node *Port1_q2_ipv4_Output_Buffer_Head = NULL;
	ipv4Node *Port1_q2_ipv4_Output_Buffer_Rear = NULL;

	ipv4Node *Port2_q1_ipv4_Output_Buffer_Head = NULL;
	ipv4Node *Port2_q1_ipv4_Output_Buffer_Rear = NULL;

	ipv4Node *Port2_q2_ipv4_Output_Buffer_Head = NULL;
	ipv4Node *Port2_q2_ipv4_Output_Buffer_Rear = NULL;

	ipv4Node *Port3_q1_ipv4_Output_Buffer_Head = NULL;
	ipv4Node *Port3_q1_ipv4_Output_Buffer_Rear = NULL;

	ipv4Node *Port3_q2_ipv4_Output_Buffer_Head = NULL;
	ipv4Node *Port3_q2_ipv4_Output_Buffer_Rear = NULL;

	int pkt_count = 0;
	ipv4Packet *packet;


	ipv4Node *node = Port1_q1_ipv4_Output_Buffer_Head;

	while (NULL != node)
	{
		packet = node->ipv4Pkt;
		fwrite(node->ipv4Pkt->complete_packet, 1, node->ipv4Pkt->pkt_len, outfile1);
		node = node->next;

	}

	node = Port1_q2_ipv4_Output_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;
		fwrite(node->ipv4Pkt->complete_packet, 1, node->ipv4Pkt->pkt_len, outfile1);
		node = node->next;

	}

	node = Port2_q1_ipv4_Output_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;
		fwrite(node->ipv4Pkt->complete_packet, 1, node->ipv4Pkt->pkt_len, outfile2);
		node = node->next;

	}

	node = Port2_q2_ipv4_Output_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;
		fwrite(node->ipv4Pkt->complete_packet, 1, node->ipv4Pkt->pkt_len, outfile2);
		node = node->next;

	}

	node = Port3_q1_ipv4_Output_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;
		fwrite(node->ipv4Pkt->complete_packet, 1, node->ipv4Pkt->pkt_len, outfile3);
		node = node->next;

	}

	node = Port3_q2_ipv4_Output_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;
		fwrite(node->ipv4Pkt->complete_packet, 1, node->ipv4Pkt->pkt_len, outfile3);
		node = node->next;

	}

}

#endif /* INPUT_THREADS_C_ */
