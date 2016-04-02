/*
 ============================================================================
 Name        : simulatorCN2.c
 Author      : saroj
 Version     :
 Copyright   : Your copyright notice
 Description : Main Simulator File. Contains functions for parsing input files,
 	 	 	 	 generating IPV4/IPV6 packets, starting the input threads to
 	 	 	 	 simulate the Router Input Links.
 ============================================================================
 */

#include "cn2simulator.h"

#include "simulator_hash_inputs.h"
//#include "simulator_inputs.h"
#include "output_threads.h"
#include "bld_routing_tbl.h"


int input_thread1_slp_time = 0;
int input_thread2_slp_time = 0;
int input_thread3_slp_time = 0;

int output_thread1_slp_time = 0;
int output_thread2_slp_time = 0;
int output_thread3_slp_time = 0;

//Input Thread Parameters
link_param input_port1_q1_param, input_port1_q2_param;
link_param input_port2_q1_param, input_port2_q2_param;
link_param input_port3_q1_param, input_port3_q2_param;

//Output Queue Link Thread Parameters
ipv4_port_param ipv4_port1_param, ipv4_port2_param, ipv4_port3_param;

//3 Threads to simulate 3 links and process data from the 3 buffers
pthread_t input_thread1, input_thread2, input_thread3, input_thread4, input_thread5, input_thread6;


//Variable to Keep track of Packets processed on each output port. Declared in output_thread.c
extern int port1_pkt_count;
extern int port2_pkt_count;
extern int port3_pkt_count;

//Variable to Keep track of Packets processed from each output queue. Declared in output_thread.c
extern int port1_q1_pkt_counter;
extern int port1_q2_pkt_counter;
extern int port2_q1_pkt_counter;
extern int port2_q2_pkt_counter;
extern int port3_q1_pkt_counter;
extern int port3_q2_pkt_counter;


//Variables To Hold Max Queue Depth
extern int port1_q1_max_q_depth;
extern int port1_q2_max_q_depth;
extern int port2_q1_max_q_depth;
extern int port2_q2_max_q_depth;
extern int port3_q1_max_q_depth;
extern int port3_q2_max_q_depth;

//VAriables to Hold Max Residence time of each Queue
extern int port1_q1_max_res_tm;
extern int port1_q2_max_res_tm;
extern int port2_q1_max_res_tm;
extern int port2_q2_max_res_tm;
extern int port3_q1_max_res_tm;
extern int port3_q2_max_res_tm;


FILE *outfile1 = NULL;
FILE *outfile2 = NULL;
FILE *outfile3 = NULL;

FILE *logfile=NULL;
FILE *route=NULL;

FILE *pkt = NULL;

extern ipv4RoutingTable *ipv4RoutingTbl;
extern ipv6RoutingTable *ipv6RoutingTbl;


extern pthread_t output_thread1;
extern pthread_t output_thread2;
extern pthread_t output_thread3;


//Mutexes For each IPV4 Queue
extern pthread_mutex_t ipv4_port1_queue1_mutex;
extern pthread_mutex_t ipv4_port1_queue2_mutex;

extern pthread_mutex_t ipv4_port2_queue1_mutex;
extern pthread_mutex_t ipv4_port2_queue2_mutex;

extern pthread_mutex_t ipv4_port3_queue1_mutex;
extern pthread_mutex_t ipv4_port3_queue2_mutex;

//Semaphore Names for each IPV4 Queue
extern sem_t ipv4_port1_sem;
extern sem_t ipv4_port2_sem;
extern sem_t ipv4_port3_sem;



/*Packet counters for each thread to keep track of where to start processing
 * Next Packet. It will start from 0 and each tile incremented by pkt_lim
 * of that link till all the packets from that link is processed.
 */

int input_link1_pkt_count;
int input_link2_pkt_count;
int input_link3_pkt_count;


unsigned char * Buffer1[MAX_FILE1_SIZE];
unsigned char * Buffer2[MAX_FILE2_SIZE];
unsigned char * Buffer3[MAX_FILE3_SIZE];

ipv4Node *Port1_q1_ipv4_Buffer_Head = NULL;
ipv4Node *Port1_q1_ipv4_Buffer_Rear = NULL;

ipv4Node *Port1_q2_ipv4_Buffer_Head = NULL;
ipv4Node *Port1_q2_ipv4_Buffer_Rear = NULL;

ipv4Node *Port2_q1_ipv4_Buffer_Head = NULL;
ipv4Node *Port2_q1_ipv4_Buffer_Rear = NULL;

ipv4Node *Port2_q2_ipv4_Buffer_Head = NULL;
ipv4Node *Port2_q2_ipv4_Buffer_Rear = NULL;

ipv4Node *Port3_q1_ipv4_Buffer_Head = NULL;
ipv4Node *Port3_q1_ipv4_Buffer_Rear = NULL;

ipv4Node *Port3_q2_ipv4_Buffer_Head = NULL;
ipv4Node *Port3_q2_ipv4_Buffer_Rear = NULL;


extern int port1_q1_total_res_time;
extern int port1_q2_total_res_time;
extern int port2_q1_total_res_time;
extern int port2_q2_total_res_time;
extern int port3_q1_total_res_time;
extern int port3_q2_total_res_time;


extern int port1_q1_total_res_items;
extern int port1_q2_total_res_items;
extern int port2_q1_total_res_items;
extern int port2_q2_total_res_items;
extern int port3_q1_total_res_items;
extern int port3_q2_total_res_items;

float calculated_port1_q1_res_time;
float calculated_port1_q2_res_time;
float calculated_port2_q1_res_time;
float calculated_port2_q2_res_time;
float calculated_port3_q1_res_time;
float calculated_port3_q2_res_time;


float calculated_port1_q1_res_items;
float calculated_port1_q2_res_items;
float calculated_port2_q1_res_items;
float calculated_port2_q2_res_items;
float calculated_port3_q1_res_items;
float calculated_port3_q2_res_items;



//unsigned char **Buffer1,**Buffer2,**Buffer3;

//int readFile(char *fname, unsigned char buffer[][PACKECT_SIZE], int maxsize)

int readFile(char *fname, unsigned char **buffer, int maxsize)
{
	int pkt_count = 0;
	int i = 0;
	FILE *fptr=NULL;

	unsigned char total_len_1st_byte, total_len_2nd_byte;

//Open The Input File To Read in Binary Mode.
	fptr = fopen(fname, "rb");
	if(NULL==fptr)
	{
		printf("Could not open %s\n", fname);
		exit(0);
	}
	else
	{
		if (pkt_count < maxsize )
		{

			unsigned char hdr_first_byte;

			while(fread(&hdr_first_byte, sizeof(unsigned char), 1, fptr))
			{
//				printf("hdr_first_byte : %x\n", hdr_first_byte);
			// Get header size
				int header_size = 4*(hdr_first_byte & 0x0F);
//				printf("\nHeader size = %d",header_size);


				unsigned char *header = (unsigned char *)malloc(header_size*sizeof(unsigned char));
				header[0] = hdr_first_byte;
				fread(header+1, sizeof(unsigned char), header_size-1, fptr);
			//printf("\n%u %u\n",header[2],header[3]);

//				printf("\n%x %x\n",header[2],header[3]);

				total_len_1st_byte = header[2];
				total_len_2nd_byte = header[3];

				int total_size = (total_len_1st_byte<<8) + total_len_2nd_byte;

//				printf("\nTotal size = %d\n",total_size);

		//		sleep(5);

			//printf("\nTotal size = %x",total_size);
				buffer[pkt_count] = (unsigned char *)malloc((total_size)*sizeof(unsigned char));

				//Copy The content of Header to buffer
				memcpy(buffer[pkt_count],header,header_size);

				free(header);
				int size_to_read = total_size - header_size;

//				printf("Rest size_to_read :%d\n", size_to_read);
				int size_read = fread(buffer[pkt_count]+header_size, sizeof(unsigned char), size_to_read, fptr);
				//sleep(5);

				pkt_count++;

			}
		}

		else
		{
			printf("%s has more packets than defined, Ignoring extra Packets\n", fname);
		}
		if (0 == pkt_count)
		{
			printf("%s is empty", fname);
		}
		printf("Total No of Packets in %s : %d\n", fname, pkt_count);
	}

	return pkt_count;

}

//This function will read each input file till end of file is encountered
//and populate each buffer with the packets from the file.
void Populate_Buffers()
{

	input_link1_pkt_count = readFile(INPUT_FILE1_NAME, Buffer1, MAX_FILE1_SIZE);
	input_link2_pkt_count = readFile(INPUT_FILE2_NAME, Buffer2, MAX_FILE2_SIZE);
	input_link3_pkt_count = readFile(INPUT_FILE3_NAME, Buffer3, MAX_FILE3_SIZE);

	printf("input_link1_pkt_count :%d\n", input_link1_pkt_count);
	printf("input_link2_pkt_count :%d\n", input_link2_pkt_count);
	printf("input_link3_pkt_count :%d\n", input_link3_pkt_count);

	//sleep(2);


}



void log_message()
{

}

/*
 * This function creates an ipv4Packet from the raw data that we read from the input file.
 */

ipv4Packet * create_ipv4_pkt(unsigned char *buffer)
{
	ipv4Packet *ipv4Pkt = NULL;

	//Variable to parse the raw data and extract info like src ip, dest ip etc.
	unsigned char *pktPtr;

	unsigned char total_len_1st_byte, total_len_2nd_byte;

	ipv4Pkt = (ipv4Packet *) malloc(sizeof(ipv4Packet));
	ipv4Pkt->version = 4;


	pktPtr = buffer;

	total_len_1st_byte = pktPtr[2];
	total_len_2nd_byte = pktPtr[3];

	//Calculate the total length of the packet and store it
	ipv4Pkt->pkt_len = (total_len_1st_byte<<8) + total_len_2nd_byte;

	//Extract the DSCP ECN byte and store it, which is 2nd byte in the IPV4 Header
	ipv4Pkt->dscp_ecn = pktPtr[1];

	//Extract the Source Ip by moving pktPtr to the starting of Source Ip
	pktPtr = pktPtr + 12;

	//strncpy(ipv4Pkt->source_ip, pktPtr, 4);
	memcpy(ipv4Pkt->source_ip, pktPtr, 4);


	//Extract the Destination Ip by moving pktPtr to the starting of Destination Ip
	pktPtr = pktPtr + 4;
	//strncpy(ipv4Pkt->dest_ip, pktPtr, 4);
	memcpy(ipv4Pkt->dest_ip, pktPtr, 4);

	//Hold the complete raw Packet which will b used to write to the file.
	ipv4Pkt->complete_packet = buffer;

	return ipv4Pkt;


}


/*This function calculates the relative speed of each link based on rate
 * limit input and transfers that to packet limit processing at a time from
 * each link
 */

/*
void calculate_threads_slp_times()
{
	short total_rate =0, total_input_rate =0, total_output_rate = 0;
	double input1_time = 0.00;
	double input2_time = 0.00;
	double input3_time = 0.00;

	double output1_time = 0.00;
	double output2_time = 0.00;
	double output3_time = 0.00;



	total_input_rate = INPUT_LINK1_SPEED + INPUT_LINK2_SPEED + INPUT_LINK3_SPEED;
	total_output_rate = OUTPUT_LINK1_SPEED + OUTPUT_LINK2_SPEED + OUTPUT_LINK3_SPEED;
	total_rate = total_input_rate + total_output_rate;

	if (0==total_rate)
	{
		fprintf(logfile, "All Link speed 0, nothing can be processed, Exiting Simulator...\n");
		exit(0);
	}

	//sprintf((char *)&input1_time, "%01lf", (double)(total_rate/INPUT_LINK1_SPEED));
	input1_time = (double)(total_rate/INPUT_LINK1_SPEED);
	input2_time = (double)(total_rate/INPUT_LINK2_SPEED);
	input3_time = (double)(total_rate/INPUT_LINK3_SPEED);

	output1_time = (double)(total_rate/OUTPUT_LINK1_SPEED);
	output2_time = (double)(total_rate/OUTPUT_LINK2_SPEED);
	output3_time = (double)(total_rate/OUTPUT_LINK3_SPEED);

	printf("input1_time :%lf\n", input1_time);
	printf("input2_time :%lf\n", input2_time);
	printf("input3_time :%lf\n", input3_time);
	printf("output1_time :%lf\n", output1_time);
	printf("output2_time :%lf\n", output2_time);
	printf("output3_time :%lf\n", output3_time);

	printf("total_rate :%d\n", total_rate);
	printf("INPUT_LINK1_SPEED :%d\n", INPUT_LINK1_SPEED);
	printf("INPUT_LINK2_SPEED :%d\n", INPUT_LINK2_SPEED);
	printf("INPUT_LINK3_SPEED :%d\n", INPUT_LINK3_SPEED);

	printf("OUTPUT_LINK1_SPEED :%d\n", OUTPUT_LINK1_SPEED);
	printf("OUTPUT_LINK2_SPEED :%d\n", OUTPUT_LINK2_SPEED);
	printf("OUTPUT_LINK3_SPEED :%d\n", OUTPUT_LINK3_SPEED);


	input_thread1_slp_time = (int)(input1_time * MULTIPLIER);
	input_thread2_slp_time = (int)(input2_time * MULTIPLIER);

	input_thread3_slp_time = (int)(input3_time * MULTIPLIER);
	output_thread1_slp_time = (int)(output1_time * MULTIPLIER);

	output_thread2_slp_time = (int)(output2_time * MULTIPLIER);
	output_thread3_slp_time = (int)(output3_time * MULTIPLIER);



	printf("input_thread1_slp_time :%d\n", input_thread1_slp_time);
	printf("input_thread2_slp_time :%d\n", input_thread2_slp_time);
	printf("input_thread3_slp_time :%d\n", input_thread3_slp_time);

	printf("output_thread1_slp_time :%d\n", output_thread1_slp_time);
	printf("output_thread2_slp_time :%d\n", output_thread2_slp_time);
	printf("output_thread3_slp_time :%d\n", output_thread3_slp_time);

//	sleep(60);


}
*/

void initialize_output_files()
{

	//Open The Output Files To Write in Binary Mode.
	outfile1 = fopen(IPV4_OUTPUT_FILE1_NAME, "wb");
	if(NULL == outfile1)
	{
		printf("Could not open %s\n", IPV4_OUTPUT_FILE1_NAME);
		exit(0);
	}

	outfile2 = fopen(IPV4_OUTPUT_FILE2_NAME, "wb");
	if(NULL == outfile2)
	{
		printf("Could not open %s\n", IPV4_OUTPUT_FILE2_NAME);
		exit(0);
	}
	outfile3 = fopen(IPV4_OUTPUT_FILE3_NAME, "wb");
	if(NULL == outfile1)
	{
		printf("Could not open %s\n", IPV4_OUTPUT_FILE3_NAME);
		exit(0);
	}

}


void addTo_IPV4_Queue_Buffer(ipv4_port_no port_no, ipv4_queue_no q_no, ipv4Packet *ipv4Pkt)
{
	ipv4Node *node = (ipv4Node *)malloc(sizeof(ipv4Node));
	node->ipv4Pkt = ipv4Pkt;
	node->next = NULL;

	switch(port_no)
		{
		case IPV4_PORT_1:
			if (IPV4_QUEUE_1 == q_no)
			{

				//Increment the Packet counter
				port1_q1_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port1_q1_pkt_counter;

				if(Port1_q1_ipv4_Buffer_Head == NULL && Port1_q1_ipv4_Buffer_Rear == NULL)
				{
					Port1_q1_ipv4_Buffer_Head = Port1_q1_ipv4_Buffer_Rear = node;
				}
				else
				{

					Port1_q1_ipv4_Buffer_Rear->next = node;
					Port1_q1_ipv4_Buffer_Rear = node;
				}

			}
			else if (IPV4_QUEUE_2 == q_no)
			{

				//Increment the Packet counter
				port1_q2_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port1_q2_pkt_counter;

				if(Port1_q2_ipv4_Buffer_Head == NULL && Port1_q2_ipv4_Buffer_Rear == NULL)
				{
					Port1_q2_ipv4_Buffer_Head = Port1_q2_ipv4_Buffer_Rear = node;
				}
				else
				{

					Port1_q2_ipv4_Buffer_Rear->next = node;
					Port1_q2_ipv4_Buffer_Rear = node;
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

				if(Port2_q1_ipv4_Buffer_Head == NULL && Port2_q1_ipv4_Buffer_Rear == NULL)
				{
					Port2_q1_ipv4_Buffer_Head = Port2_q1_ipv4_Buffer_Rear = node;
				}
				else
				{

					Port2_q1_ipv4_Buffer_Rear->next = node;
					Port2_q1_ipv4_Buffer_Rear = node;
				}

			}
			else if (IPV4_QUEUE_2 == q_no)
			{
				//Increment the Packet counter
				port2_q2_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port2_q2_pkt_counter;

				if(Port2_q2_ipv4_Buffer_Head == NULL && Port2_q2_ipv4_Buffer_Rear == NULL)
				{
					Port2_q2_ipv4_Buffer_Head = Port2_q2_ipv4_Buffer_Rear = node;
				}
				else
				{

					Port2_q2_ipv4_Buffer_Rear->next = node;
					Port2_q2_ipv4_Buffer_Rear = node;
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

				if(Port3_q1_ipv4_Buffer_Head == NULL && Port3_q1_ipv4_Buffer_Rear == NULL)
				{
					Port3_q1_ipv4_Buffer_Head = Port3_q1_ipv4_Buffer_Rear = node;
				}
				else
				{

					Port3_q1_ipv4_Buffer_Rear->next = node;
					Port3_q1_ipv4_Buffer_Rear = node;
				}

			}
			else if (IPV4_QUEUE_2 == q_no)
			{
				//Increment the Packet counter
				port3_q2_pkt_counter++;

				//Set THe Packet counter which can be used to keep track of depth of Queue
				ipv4Pkt->pkt_counter = port3_q2_pkt_counter;

				if(Port3_q2_ipv4_Buffer_Head == NULL && Port3_q2_ipv4_Buffer_Rear == NULL)
				{
					Port3_q2_ipv4_Buffer_Head = Port3_q2_ipv4_Buffer_Rear = node;
				}
				else
				{

					Port3_q2_ipv4_Buffer_Rear->next = node;
					Port3_q2_ipv4_Buffer_Rear = node;
				}
			}

			break;
		}

}

/* Process each Ipv4 Packet - Navneet
 * Based on destination Ip, check the Ipv4 routing table and process the packet
*/

//Assigned - Navneet
int process_ipv4Packet(ipv4Packet *packet)
{
	int q_no = 0;
	int i;

	unsigned char pkt_dest_ip[4];

	unsigned char dest_ntwork[4];

	unsigned char queue_no = 0x00;
	unsigned char port_no = 0x00;

	int tbl_exist = 0;

	int no_of_byte_compare = 0;

	ipv4RoutingTable *ipv4Entry;


	//This is to take care of the last entry in routing table where the packet has to go to default port
	ipv4RoutingTable *ipv4EntryPrev = NULL;

	ipv4Entry = ipv4RoutingTbl;

//	printf("Processing Packet process_ipv4Packet\n");
	if (ipv4Entry == NULL)
	{
		printf("No Routing Table, Unable to Process Packet\n");
		return tbl_exist;
	}
	while (ipv4Entry !=NULL)
	{
/*Check if there is an entry in Routing Table for the destination address in the packet
 *
 */
		/*
		 * Perform Masking using the Destination Network Mask and comapre the Result with
		 * Forwarding Table Destination network to get the port no and queue no
		 */

		//Copy the Packet Destination IP to perform Masking and comparison
		memcpy(pkt_dest_ip, packet->dest_ip, 4);
		no_of_byte_compare = 0;

//		memcpy(dest_ntwork, ipv4Entry->info.dst_ntwrk, 4)
		for (i=0; i<4; i++)
		{
			pkt_dest_ip[i] = pkt_dest_ip[i] & ipv4Entry->info.dst_ntwrk_mask[i];

			if (ipv4Entry->info.dst_ntwrk_mask[i] > 0)
			{
				no_of_byte_compare++;
			}

//			printf("After Masking\n");
//			printf("pkt_dest_ip[%d] :%x\n", i, pkt_dest_ip[i]);
		}
		//sleep(2);

//Perform Longest Prefix Match, start with all 4 bytes then 3 bytes and so on

		if (4 == no_of_byte_compare)
		{
			if ((pkt_dest_ip[0] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[0])) && \
					(pkt_dest_ip[1] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[1])) && \
					(pkt_dest_ip[2] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[2])) && \
					(pkt_dest_ip[3] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[3])))
			{
				//Found Matching Entry break out of the Loop.
				port_no = (unsigned char)(ipv4Entry->info.output_port);
				queue_no = (unsigned char)(ipv4Entry->info.output_port_queue);
				break;

			}
			else
			{
				//It didn't match, move to the next entry and check
				ipv4Entry = ipv4Entry->next;
				continue;
			}
		}
		else if (3 == no_of_byte_compare)
		{
			if ((pkt_dest_ip[0] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[0])) && \
				(pkt_dest_ip[1] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[1])) && \
				(pkt_dest_ip[2] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[2])))
			{
				port_no = (unsigned char)(ipv4Entry->info.output_port);
				queue_no = (unsigned char)(ipv4Entry->info.output_port_queue);
				break;
			}
			else
			{
				//It didn't match, move to the next entry and check
				ipv4Entry = ipv4Entry->next;
				continue;
			}
		}
		else if (2 == no_of_byte_compare)
		{
			if ((pkt_dest_ip[0] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[0])) && \
				(pkt_dest_ip[1] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[1])))

			{
				port_no = (unsigned char)(ipv4Entry->info.output_port);
				queue_no = (unsigned char)(ipv4Entry->info.output_port_queue);
				break;
			}
			else
			{
				//It didn't match, move to the next entry and check
				ipv4Entry = ipv4Entry->next;
				continue;
			}
		}
		else if (1 == no_of_byte_compare)
		{

			if (pkt_dest_ip[0] == (unsigned) (unsigned char)(ipv4Entry->info.dst_ntwrk[0]))

			{
				port_no = (unsigned char)(ipv4Entry->info.output_port);
				queue_no = (unsigned char)(ipv4Entry->info.output_port_queue);
				break;
			}
			else
			{
				//It didn't match, move to the next entry and check
				ipv4Entry = ipv4Entry->next;
				continue;
			}
		}

		else
		{

			/*
			 * No matching entry found after scanning all entries.
			 * route it to the default port & queue, which is the last entry because of sorting
			 * on Destination Network Mask
			 */
			port_no = (unsigned char)(ipv4Entry->info.output_port);
			queue_no = (unsigned char)(ipv4Entry->info.output_port_queue);
			break;

		}


	}

//	printf("Found ROuting Info, Adding Packet to Queue %d\n", ipv4Entry->info.output_port_queue);

	if (0x01 == port_no)
	{
		if (0x01 == queue_no)
		{

/* Add The Packet to Port1_q1_ipv4_Buffer
 *
 */
			addTo_IPV4_Queue_Buffer(IPV4_PORT_1, IPV4_QUEUE_1, packet);
/*
			pthread_mutex_lock (&ipv4_port1_queue1_mutex);
			ipv4Enqueue(IPV4_PORT_1, IPV4_QUEUE_1, packet);
//			printf("Added Packet to IPV4_PORT_1 IPV4_QUEUE_1\n");

			pthread_mutex_unlock(&ipv4_port1_queue1_mutex);

			//Semaphore Post to signal the Thread which processes Packets from Output Port 1
			sem_post(&ipv4_port1_sem);
*/
		}
		else if (0x02 == queue_no)
		{
			addTo_IPV4_Queue_Buffer(IPV4_PORT_1, IPV4_QUEUE_2, packet);

/*
			pthread_mutex_lock (&ipv4_port1_queue2_mutex);
			ipv4Enqueue(IPV4_PORT_1, IPV4_QUEUE_2, packet);
//			printf("Added Packet to IPV4_PORT_1 IPV4_QUEUE_2\n");

			pthread_mutex_unlock(&ipv4_port1_queue2_mutex);

			//Semaphore Post to signal the Thread which processes Packets from Output Port 1
			sem_post(&ipv4_port1_sem);
*/
		}
/*		else
		{
			//Code for More queues per Port will go here
		}
*/

	}
//	if (0x02 == ((unsigned) (unsigned char)(ipv4Entry->info.output_port)))
	else if (0x02 == port_no)
	{
		if (0x01 == queue_no)
		{
			addTo_IPV4_Queue_Buffer(IPV4_PORT_2, IPV4_QUEUE_1, packet);
/*
			pthread_mutex_lock (&ipv4_port2_queue1_mutex);
			ipv4Enqueue(IPV4_PORT_2, IPV4_QUEUE_1, packet);
//			printf("Added Packet to IPV4_PORT_2 IPV4_QUEUE_1\n");

			pthread_mutex_unlock(&ipv4_port2_queue1_mutex);

			//Semaphore Post to signal the Thread which processes Packets from Output Port 2
			sem_post(&ipv4_port2_sem);

*/
		}
		else if (0x02 == queue_no)
		{
			addTo_IPV4_Queue_Buffer(IPV4_PORT_2, IPV4_QUEUE_2, packet);

/*
			pthread_mutex_lock (&ipv4_port2_queue2_mutex);
			ipv4Enqueue(IPV4_PORT_2, IPV4_QUEUE_2, packet);
//			printf("Added Packet to IPV4_PORT_2 IPV4_QUEUE_2\n");

			pthread_mutex_unlock(&ipv4_port2_queue2_mutex);

			//Semaphore Post to signal the Thread which processes Packets from Output Port 2
			sem_post(&ipv4_port2_sem);
*/
		}
/*		else
		{
			//Code for More queues per Port will go here
		}
*/
	}

//	if (0x03 == ((unsigned) (unsigned char)(ipv4Entry->info.output_port)))
	else if (0x03 == port_no)
	{
		if (0x01 == queue_no)
		{
			addTo_IPV4_Queue_Buffer(IPV4_PORT_3, IPV4_QUEUE_1, packet);
/*
			pthread_mutex_lock (&ipv4_port3_queue1_mutex);
			ipv4Enqueue(IPV4_PORT_3, IPV4_QUEUE_1, packet);
//			printf("Added Packet to IPV4_PORT_3 IPV4_QUEUE_1\n");

			pthread_mutex_unlock(&ipv4_port3_queue1_mutex);

			//Semaphore Post to signal the Thread which processes Packets from Output Port 3
			sem_post(&ipv4_port3_sem);
*/
		}
		else if (0x02 == queue_no)
		{
			addTo_IPV4_Queue_Buffer(IPV4_PORT_3, IPV4_QUEUE_2, packet);

/*
			pthread_mutex_lock (&ipv4_port3_queue2_mutex);
			ipv4Enqueue(IPV4_PORT_3, IPV4_QUEUE_2, packet);
//			printf("Added Packet to IPV4_PORT_3 IPV4_QUEUE_2\n");

			pthread_mutex_unlock(&ipv4_port3_queue2_mutex);

			//Semaphore Post to signal the Thread which processes Packets from Output Port 3
			sem_post(&ipv4_port3_sem);
*/
		}

	}

	else
	{
		printf("Could Not Find Routing Info for Packet\n");
		sleep(5);
	}
	
//	sleep(1);
	return q_no;
}

/* Process each Ipv6 Packet -
 * Based on destination Ip, check the Ipv6 routing table and process the packet
 Assigned - Navneet
 */
int process_ipv6Packet(ipv6Packet *packet)
{
	return TRUE;
}


void process_port1_q1_packets(link_param *lparam)
{
	int count_to_sleep = 0;

	int pkt_count = 0;


	ipv4Packet *packet;

	link_param *lp;
	lp = lparam;


	ipv4Node *node = Port1_q1_ipv4_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;

		pthread_mutex_lock (&ipv4_port1_queue1_mutex);
		ipv4Enqueue(IPV4_PORT_1, IPV4_QUEUE_1, packet);
//			printf("Added Packet to IPV4_PORT_1 IPV4_QUEUE_1\n");

		pthread_mutex_unlock(&ipv4_port1_queue1_mutex);

		//Semaphore Post to signal the Thread which processes Packets from Output Port 1
		sem_post(&ipv4_port1_sem);

		pkt_count++;
		node = node->next;

		count_to_sleep++;

		if (count_to_sleep == lp->pkt_per_second)
		{
			//Sent Lambda Packets, sleep for 1 second
			sleep(1);
			//Reset count_to_sleep
			count_to_sleep = 0;
		}

			//Sleep For Calculated time
//			usleep(lp->slp_time);


	}

	printf("Processed all %d Port1 Q1 Packets\n", pkt_count);
}

void process_port1_q2_packets(link_param *lparam)
{
	int count_to_sleep = 0;

	int pkt_count = 0;
	ipv4Packet *packet;

	link_param *lp;
	lp = lparam;


	ipv4Node *node = Port1_q2_ipv4_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;

		pthread_mutex_lock (&ipv4_port1_queue2_mutex);
		ipv4Enqueue(IPV4_PORT_1, IPV4_QUEUE_2, packet);
//			printf("Added Packet to IPV4_PORT_1 IPV4_QUEUE_1\n");

		pthread_mutex_unlock(&ipv4_port1_queue2_mutex);

		//Semaphore Post to signal the Thread which processes Packets from Output Port 1
		sem_post(&ipv4_port1_sem);
		pkt_count++;

		node = node->next;

		count_to_sleep++;

		if (count_to_sleep == lp->pkt_per_second)
		{
			//Sent Lambda Packets, sleep for 1 second
			sleep(1);
			//Reset count_to_sleep
			count_to_sleep = 0;
		}

			//Sleep For Calculated time
//			usleep(lp->slp_time);


	}

	printf("Processed all %d Port1 Q2 Packets\n", pkt_count);
}

void process_port2_q1_packets(link_param *lparam)
{
	int count_to_sleep = 0;

	int pkt_count = 0;
	ipv4Packet *packet;

	link_param *lp;
	lp = lparam;


	ipv4Node *node = Port2_q1_ipv4_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;

		pthread_mutex_lock (&ipv4_port2_queue1_mutex);
		ipv4Enqueue(IPV4_PORT_2, IPV4_QUEUE_1, packet);
//			printf("Added Packet to IPV4_PORT_1 IPV4_QUEUE_1\n");

		pthread_mutex_unlock(&ipv4_port2_queue1_mutex);

		//Semaphore Post to signal the Thread which processes Packets from Output Port 1
		sem_post(&ipv4_port2_sem);
		pkt_count++;

		node = node->next;

		count_to_sleep++;

		if (count_to_sleep == lp->pkt_per_second)
		{
			//Sent Lambda Packets, sleep for 1 second
			sleep(1);
			//Reset count_to_sleep
			count_to_sleep = 0;
		}

			//Sleep For Calculated time
//			usleep(lp->slp_time);


	}

	printf("Processed all %d Port2 Q1 Packets\n", pkt_count);
}

void process_port2_q2_packets(link_param *lparam)
{
	int count_to_sleep = 0;

	int pkt_count = 0;
	ipv4Packet *packet;

	link_param *lp;
	lp = lparam;


	ipv4Node *node = Port2_q2_ipv4_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;

		pthread_mutex_lock (&ipv4_port2_queue2_mutex);
		ipv4Enqueue(IPV4_PORT_2, IPV4_QUEUE_2, packet);
//			printf("Added Packet to IPV4_PORT_1 IPV4_QUEUE_1\n");

		pthread_mutex_unlock(&ipv4_port2_queue2_mutex);

		//Semaphore Post to signal the Thread which processes Packets from Output Port 1
		sem_post(&ipv4_port2_sem);
		pkt_count++;

		node = node->next;

		count_to_sleep++;

		if (count_to_sleep == lp->pkt_per_second)
		{
			//Sent Lambda Packets, sleep for 1 second
			sleep(1);
			//Reset count_to_sleep
			count_to_sleep = 0;
		}
			//Sleep For Calculated time
//			usleep(lp->slp_time);


	}

	printf("Processed all %d Port2 Q2 Packets\n", pkt_count);
}

void process_port3_q1_packets(link_param *lparam)
{
	int count_to_sleep = 0;

	int pkt_count = 0;
	ipv4Packet *packet;

	link_param *lp;
	lp = lparam;


	ipv4Node *node = Port3_q1_ipv4_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;

		pthread_mutex_lock (&ipv4_port3_queue1_mutex);
		ipv4Enqueue(IPV4_PORT_3, IPV4_QUEUE_1, packet);
//			printf("Added Packet to IPV4_PORT_1 IPV4_QUEUE_1\n");

		pthread_mutex_unlock(&ipv4_port3_queue1_mutex);

		//Semaphore Post to signal the Thread which processes Packets from Output Port 1
		sem_post(&ipv4_port3_sem);
		pkt_count++;

		node = node->next;

		count_to_sleep++;

		if (count_to_sleep == lp->pkt_per_second)
		{
			//Sent Lambda Packets, sleep for 1 second
			sleep(1);
			//Reset count_to_sleep
			count_to_sleep = 0;
		}
			//Sleep For Calculated time
//			usleep(lp->slp_time);


	}

	printf("Processed all %d Port3 Q1 Packets\n", pkt_count);
}

void process_port3_q2_packets(link_param *lparam)
{
	int count_to_sleep = 0;

	int pkt_count = 0;
	ipv4Packet *packet;

	link_param *lp;
	lp = lparam;


	ipv4Node *node = Port3_q2_ipv4_Buffer_Head;
	while (NULL != node)
	{
		packet = node->ipv4Pkt;

		pthread_mutex_lock (&ipv4_port3_queue2_mutex);
		ipv4Enqueue(IPV4_PORT_3, IPV4_QUEUE_2, packet);
//			printf("Added Packet to IPV4_PORT_1 IPV4_QUEUE_1\n");

		pthread_mutex_unlock(&ipv4_port3_queue2_mutex);

		//Semaphore Post to signal the Thread which processes Packets from Output Port 1
		sem_post(&ipv4_port3_sem);
		pkt_count++;

		node = node->next;

		count_to_sleep++;

		if (count_to_sleep == lp->pkt_per_second)
		{
			//Sent Lambda Packets, sleep for 1 second
			sleep(1);
			//Reset count_to_sleep
			count_to_sleep = 0;
		}

			//Sleep For Calculated time
//			usleep(lp->slp_time);


	}

	printf("Processed all %d Port3 Q2 Packets\n", pkt_count);
}

void *input_link_thread_function( void *ptr )
{

	int datetime_diff_ms;
	link_param *lp;

	struct timeval t0, t1;

	lp = (link_param *)ptr;
	
//#ifdef DEBUG

//	printf("Thread For Port %d Queue %d %s %d \n", lp->port_no,lp->q_no, __FILE__, __LINE__);
//	gettimeofday(&t0, NULL);

/*
 * To simulate speed limit, make the thread to sleep for the calculated time
 */
//	usleep(lparam->slp_time);

	switch( lp->port_no )
	{
		case IPV4_PORT_1:
			if(IPV4_QUEUE_1 == lp->q_no)
			{
				process_port1_q1_packets(lp);
			}
			else if(IPV4_QUEUE_2 == lp->q_no)
			{
				process_port1_q2_packets(lp);
			}
			break;

		case IPV4_PORT_2:
			if(IPV4_QUEUE_1 == lp->q_no)
			{
				process_port2_q1_packets(lp);
			}
			else if(IPV4_QUEUE_2 == lp->q_no)
			{
				process_port2_q2_packets(lp);
			}
			break;


		case IPV4_PORT_3:
			if(IPV4_QUEUE_1 == lp->q_no)
			{
				process_port3_q1_packets(lp);
			}
			else if(IPV4_QUEUE_2 == lp->q_no)
			{
				process_port3_q2_packets(lp);
			}
			break;

	}

	return ptr;
}

//void process_link1_packets(link_param *lparam)
void process_link1_packets()
{

	int i;
	unsigned char hdr_first_byte;
	int src_counter = 0;
	int dst_counter = 0;

	ipv4Packet *ipv4Pkt;
	ipv6Packet *ipv6Pkt;

//	link_param *lp;
//	lp = lparam;

	int output_q_no = 0;


	for(i=0; i<input_link1_pkt_count; i++)
	{
//Sleep For Calculated time
//		usleep(lp->slp_time);

		hdr_first_byte = Buffer1[i][0];
		unsigned char *pktPtr = Buffer1[i];

//		printf ("IP Header 1st Byte :%d \n", hdr_first_byte);
		if ((hdr_first_byte >> 4) == 4)
		{
//			printf("Found IPv4 Packet \n");
			ipv4Pkt = create_ipv4_pkt(pktPtr);

			output_q_no = process_ipv4Packet(ipv4Pkt);


			//Sleep For Calculated time
//			usleep(lp->slp_time);

		}
		else if ((hdr_first_byte >> 4) == 6)
			printf("Found IPv6 Packet \n");



	}
	
	printf("Processed all %d Link1 Packets\n", i);
}

//void process_link2_packets(link_param *lparam)
void process_link2_packets()
{

	int i;
	unsigned char hdr_first_byte;
	int src_counter = 0;
	int dst_counter = 0;

	ipv4Packet *ipv4Pkt;
	ipv6Packet *ipv6Pkt;

//	link_param *lp = lparam;


	for(i=0; i<input_link2_pkt_count; i++)
	{


		hdr_first_byte = Buffer2[i][0];
		unsigned char *pktPtr = Buffer2[i];

//		printf ("IP Header 1st Byte :%d \n", hdr_first_byte);
		if ((hdr_first_byte >> 4) == 4)
		{
//			printf("Found IPv4 Packet \n");

			ipv4Pkt = create_ipv4_pkt(pktPtr);

			process_ipv4Packet(ipv4Pkt);

	//		sleep(10);
		//	printf("procesed link 2 packet %d\n", (i+1));
			//Sleep For Calculated time
//			usleep(lp->slp_time);

		}
		else if ((hdr_first_byte >> 4) == 6)
		{
			printf("Found IPv6 Packet \n");
		}



	}
	printf("Processed all %d Link2 Packets\n", i);
}

//void process_link3_packets(link_param *lparam)
void process_link3_packets()
{

	int i;
	unsigned char hdr_first_byte;
	int src_counter = 0;
	int dst_counter = 0;


	ipv4Packet *ipv4Pkt;
	ipv6Packet *ipv6Pkt;

//	link_param *lp = lparam;


	for(i=0; i<input_link3_pkt_count; i++)
	{


		hdr_first_byte = Buffer3[i][0];
		unsigned char *pktPtr = Buffer3[i];

//		printf ("IP Header 1st Byte :%d \n", hdr_first_byte);
		if ((hdr_first_byte >> 4) == 4)
		{
//			printf("Found IPv4 Packet \n");
			ipv4Pkt = create_ipv4_pkt(pktPtr);

			process_ipv4Packet(ipv4Pkt);

	//		sleep(10);
		//	printf("procesed link 3 packet %d\n", (i+1));

			//Sleep For Calculated time
	//		usleep(lp->slp_time);

		}
		else if ((hdr_first_byte >> 4) == 6)
		{
			printf("Found IPv6 Packet \n");
		}

	}
	printf("Processed all %d Link3 Packets %s %d\n", i, __FILE__, __LINE__);
}

int calculate_Remainder_Pkt_Count(int total_pkt_count, int rate)
{
	int remainder = total_pkt_count % rate;
	return remainder;
}


int start_input_threads()
{

	 int  iret1, iret2, iret3, iret4, iret5, iret6;

	 //Set Output Thread Parameters as well, Later on we can organize

	 	 ipv4_port1_param.speed = OUTPUT_LINK1_QUEUE1_SPEED;
	 	 ipv4_port2_param.speed = OUTPUT_LINK2_QUEUE1_SPEED;
	 	 ipv4_port3_param.speed = OUTPUT_LINK3_QUEUE1_SPEED;

//Set TEach Thread Parameters
	 input_port1_q1_param.port_no = IPV4_PORT_1;
	 input_port1_q1_param.q_no = IPV4_QUEUE_1;
	 input_port1_q1_param.pkt_per_second = INPUT_LINK1_QUEUE1_SPEED;
	 input_port1_q1_param.remainder = calculate_Remainder_Pkt_Count(port1_q1_pkt_counter, INPUT_LINK1_QUEUE1_SPEED);

	 input_port1_q2_param.port_no = IPV4_PORT_1;
	 input_port1_q2_param.q_no = IPV4_QUEUE_2;
	 input_port1_q2_param.pkt_per_second = INPUT_LINK1_QUEUE2_SPEED;
	 input_port1_q2_param.remainder = calculate_Remainder_Pkt_Count(port1_q2_pkt_counter, INPUT_LINK1_QUEUE2_SPEED);

	 input_port2_q1_param.port_no = IPV4_PORT_2;
	 input_port2_q1_param.q_no = IPV4_QUEUE_1;
	 input_port2_q1_param.pkt_per_second = INPUT_LINK2_QUEUE1_SPEED;
	 input_port2_q1_param.remainder = calculate_Remainder_Pkt_Count(port2_q1_pkt_counter, INPUT_LINK2_QUEUE1_SPEED);

	 input_port2_q2_param.port_no = IPV4_PORT_2;
	 input_port2_q2_param.q_no = IPV4_QUEUE_2;
	 input_port2_q2_param.pkt_per_second = INPUT_LINK2_QUEUE2_SPEED;
	 input_port2_q2_param.remainder = calculate_Remainder_Pkt_Count(port2_q2_pkt_counter, INPUT_LINK2_QUEUE2_SPEED);

	 input_port3_q1_param.port_no = IPV4_PORT_3;
	 input_port3_q1_param.q_no = IPV4_QUEUE_1;
	 input_port3_q1_param.pkt_per_second = INPUT_LINK3_QUEUE1_SPEED;
	 input_port3_q1_param.remainder = calculate_Remainder_Pkt_Count(port3_q1_pkt_counter, INPUT_LINK3_QUEUE1_SPEED);

	 input_port3_q2_param.port_no = IPV4_PORT_3;
	 input_port3_q2_param.q_no = IPV4_QUEUE_2;
	 input_port3_q2_param.pkt_per_second = INPUT_LINK3_QUEUE2_SPEED;
	 input_port3_q2_param.remainder = calculate_Remainder_Pkt_Count(port3_q2_pkt_counter, INPUT_LINK3_QUEUE2_SPEED);


//	 input_link1_param.slp_time = input_thread1_slp_time;

//	 calculate_rate_limit();

	 iret1 = pthread_create( &input_thread1, NULL, input_link_thread_function, (void *)&input_port1_q1_param );
	 if (iret1)
	 {
		 printf("Error - pthread_create() thread1 return code: %d\n",iret1);
		 return FALSE;
	 }

	 iret2 = pthread_create( &input_thread2, NULL, input_link_thread_function, (void *)&input_port1_q2_param);
	 if (iret2)
	 {
	 	printf("Error - pthread_create() thread2 return code: %d\n",iret2);
	 	return FALSE;
	 }

	iret3 = pthread_create( &input_thread3, NULL, input_link_thread_function, (void *)&input_port2_q1_param );
	if (iret3)
	{
		printf("Error - pthread_create() thread3 return code: %d\n",iret3);
		return FALSE;
	}

	iret4 = pthread_create( &input_thread4, NULL, input_link_thread_function, (void *)&input_port2_q2_param );
	if (iret4)
	{
		printf("Error - pthread_create() thread4 return code: %d\n",iret3);
		return FALSE;
	}

	iret4 = pthread_create( &input_thread5, NULL, input_link_thread_function, (void *)&input_port3_q1_param );
	if (iret4)
	{
		printf("Error - pthread_create() thread5 return code: %d\n",iret3);
		return FALSE;
	}

	iret4 = pthread_create( &input_thread6, NULL, input_link_thread_function, (void *)&input_port3_q2_param );
	if (iret4)
	{
		printf("Error - pthread_create() thread6 return code: %d\n",iret3);
		return FALSE;
	}

	return TRUE;
}

void make_report()
{

//	printf("Mean # items in Residence(r) Port 1 Queue 1(Observerd) :%f\n", (port1_q1_total_res_items/port1_q1_total_res_time));
	double port1_q1_mean_item_res = 0.0, port1_q2_mean_item_res = 0.0;
	double port2_q1_mean_item_res = 0.0, port2_q2_mean_item_res = 0.0;
	double port3_q1_mean_item_res = 0.0, port3_q2_mean_item_res = 0.0;

	double port1_q1_mean_item_res_by_item = 0.0, port1_q2_mean_item_res_by_item = 0.0;
	double port2_q1_mean_item_res_by_item = 0.0, port2_q2_mean_item_res_by_item = 0.0;
	double port3_q1_mean_item_res_by_item = 0.0, port3_q2_mean_item_res_by_item = 0.0;


	double port1_q1_mean_res_time = 0.0, port1_q2_mean_res_time = 0.0;
	double port2_q1_mean_res_time = 0.0, port2_q2_mean_res_time = 0.0;
	double port3_q1_mean_res_time = 0.0, port3_q2_mean_res_time = 0.0;

	//Mean Item Resident when Divided by Total Residence Time. Not sure what to be used or will it be total time
/*	if (port1_q1_total_res_time > 0)
		port1_q1_mean_item_res = (double)(port1_q1_total_res_items)/(double)(port1_q1_total_res_time);

	if (port1_q2_total_res_time > 0)
		port1_q2_mean_item_res = (double)(port1_q2_total_res_items)/(double)(port1_q2_total_res_time);

	if (port2_q1_total_res_time > 0)
		port2_q1_mean_item_res = (double)(port2_q1_total_res_items)/(double)(port2_q1_total_res_time);

	if (port2_q2_total_res_time > 0)
		port2_q2_mean_item_res = (double)(port2_q2_total_res_items)/(double)(port2_q2_total_res_time);

	if (port3_q1_total_res_time > 0)
		port3_q1_mean_item_res = (double)(port3_q1_total_res_items)/(double)(port3_q1_total_res_time);

	if (port3_q2_total_res_time > 0)
		port3_q2_mean_item_res = (double)(port3_q2_total_res_items)/(double)(port3_q2_total_res_time);
*/
/*
 * Mean Item Resident and Mean Time in Residence
 * Check if we had something in the queue
*/
	if (port1_q1_pkt_counter > 0)
	{
		port1_q1_mean_item_res_by_item = (double)(port1_q1_total_res_items)/(double)(port1_q1_pkt_counter);
		port1_q1_mean_res_time = (double)(port1_q1_total_res_time)/(double)(port1_q1_pkt_counter);
	}


	if (port1_q2_pkt_counter > 0)
	{
		port1_q2_mean_item_res_by_item = (double)(port1_q2_total_res_items)/(double)(port1_q2_pkt_counter);
		port1_q2_mean_res_time = (double)(port1_q2_total_res_time)/(double)(port1_q2_pkt_counter);
	}

	if (port2_q1_pkt_counter > 0)
	{
		port2_q1_mean_item_res_by_item = (double)(port2_q1_total_res_items)/(double)(port2_q1_pkt_counter);
		port2_q1_mean_res_time = (double)(port2_q1_total_res_time)/(double)(port2_q1_pkt_counter);
	}


	if (port2_q2_pkt_counter > 0)
	{
		port2_q2_mean_item_res_by_item = (double)(port2_q2_total_res_items)/(double)(port2_q2_pkt_counter);
		port2_q2_mean_res_time = (double)(port2_q2_total_res_time)/(double)(port2_q2_pkt_counter);
	}


	if (port3_q1_pkt_counter > 0)
	{
		port3_q1_mean_item_res_by_item = (double)(port3_q1_total_res_items)/(double)(port3_q1_pkt_counter);
		port3_q1_mean_res_time = (double)(port3_q1_total_res_time)/(double)(port3_q1_pkt_counter);
	}


	if (port3_q2_pkt_counter > 0)
	{
		port3_q2_mean_item_res_by_item = (double)(port3_q2_total_res_items)/(double)(port3_q2_pkt_counter);
		port3_q2_mean_res_time = (double)(port3_q2_total_res_time)/(double)(port3_q2_pkt_counter);
	}



	printf("\n\n=================================================================================================\n");
	printf("                                              REPORT                                                     ");
	printf("\n=================================================================================================\n");
	
	printf("No Of Packets Handled From Input Port 1 :%d\n", input_link1_pkt_count);
	printf("No Of Packets Handled From Input Port 2 :%d\n", input_link2_pkt_count);
	printf("No Of Packets Handled From Input Port 3 :%d\n", input_link3_pkt_count);
	
	printf("\n");

//Packets per Port
	printf("No Of Packets Sent On Output Link 1 :%d\n", port1_pkt_count);
	printf("No Of Packets Sent On Output Link 2 :%d\n", port2_pkt_count);
	printf("No Of Packets Sent On Output Link 3 :%d\n", port3_pkt_count);
	printf("Total Packets Sent on All Output Links :%d\n", (port1_pkt_count + port2_pkt_count + port3_pkt_count));

	printf("\n");

//Packets Per Queue
	printf("No Of Packets Sent On Port 1 Queue 1 :%d\n", port1_q1_pkt_counter);

	printf("No Of Packets Sent On Port 1 Queue 2 :%d\n", port1_q2_pkt_counter);
	printf("No Of Packets Sent On Port 2 Queue 1 :%d\n", port2_q1_pkt_counter);
	printf("No Of Packets Sent On Port 2 Queue 2 :%d\n", port2_q2_pkt_counter);
	printf("No Of Packets Sent On Port 3 Queue 1 :%d\n", port3_q1_pkt_counter);
	printf("No Of Packets Sent On Port 3 Queue 2 :%d\n", port3_q2_pkt_counter);
	printf("\n");



//	printf("Mean # items in Residence(r) Port 1 Queue 1(Observerd) :%f\n", port1_q1_mean_item_res);
	printf("Mean # items in Residence(r) Port 1 Queue 1(Calculated) :%f\n", calculated_port1_q1_res_items);
	printf("Mean # items in Residence(r) Port 1 Queue 1(Observerd) :%f\n", port1_q1_mean_item_res_by_item);

	printf("Maximum No Of Items In Residence(R) Port 1 Queue 1 :%d\n", port1_q1_max_q_depth);
	printf("Mean Time in Residence(Tr) Port 1 Queue 1(Calculated) :%f nano sec\n", calculated_port1_q1_res_time);
	printf("Mean Time in Residence(Tr) Port 1 Queue 1(Observerd) :%f nano sec\n", port1_q1_mean_res_time);
	printf("Maximum Time In Residence(TR) Port 1 Queue 1 :%d nano sec\n", port1_q1_max_res_tm);

	printf("\n");

//	printf("Mean # items in Residence(r) Port 1 Queue 2(Observerd) :%f\n", port1_q2_mean_item_res);
	printf("Mean # items in Residence(r) Port 1 Queue 2(Calculated) :%f\n", calculated_port1_q2_res_items);
	printf("Mean # items in Residence(r) Port 1 Queue 2(Observerd)  :%f\n", port1_q2_mean_item_res_by_item);
	printf("Maximum No Of Items In Residence(R) Port 1 Queue 2 :%d\n", port1_q2_max_q_depth);
	printf("Mean Time in Residence(Tr) Port 1 Queue 2(Calculated) :%f nano sec\n", calculated_port1_q2_res_time);
	printf("Mean Time in Residence(Tr) Port 1 Queue 2(Observerd) :%f nano sec\n", port1_q2_mean_res_time);
	printf("Maximum Time In Residence(TR) Port 1 Queue 2 :%d nano sec\n", port1_q2_max_res_tm);

	printf("\n");

	printf("Mean # items in Residence(r) Port 2 Queue 1(Calculated) :%f\n", calculated_port2_q1_res_items);
	//printf("Mean # items in Residence(r) Port 2 Queue 1(Observerd) :%f\n", port2_q1_mean_item_res);
	printf("Mean # items in Residence(r) Port 2 Queue 1(Observerd)  :%f\n", port2_q1_mean_item_res_by_item);
	printf("Maximum No Of Items In Residence(R) Port 2 Queue 1 :%d\n", port2_q1_max_q_depth);
	printf("Mean Time in Residence(Tr) Port 2 Queue 1(Calculated) :%f nano sec\n", calculated_port2_q1_res_time);
	printf("Mean Time in Residence(Tr) Port 2 Queue 1(Observerd) :%f nano sec\n", port2_q1_mean_res_time);
	printf("Maximum Time In Residence(TR) Port 1 Queue 1 :%d nano sec\n", port2_q1_max_res_tm);

	printf("\n");

	//printf("Mean # items in Residence(r) Port 2 Queue 2(Observerd) :%f\n", port2_q2_mean_item_res);
	printf("Mean # items in Residence(r) Port 2 Queue 2(Calculated) :%f\n", calculated_port2_q2_res_items);
	printf("Mean # items in Residence(r) Port 2 Queue 2(Observerd)  :%f\n", port2_q2_mean_item_res_by_item);
	printf("Maximum No Of Items In Residence(R) Port 2 Queue 2 :%d\n", port2_q2_max_q_depth);
	printf("Mean Time in Residence(Tr) Port 2 Queue 2(Calculated) :%f nano sec\n", calculated_port2_q2_res_time);
	printf("Mean Time in Residence(Tr) Port 2 Queue 2(Observerd) :%f nano sec\n", port2_q2_mean_res_time);
	printf("Maximum Time In Residence(TR) Port 2 Queue 2 :%d nano sec\n", port2_q2_max_res_tm);

	printf("\n");

	//printf("Mean # items in Residence(r) Port 3 Queue 1(Observerd) :%f\n", port3_q1_mean_item_res);
	printf("Mean # items in Residence(r) Port 3 Queue 1(Calculated) :%f\n", calculated_port3_q1_res_items);
	printf("Mean # items in Residence(r) Port 3 Queue 1(Observerd)  :%f\n", port3_q1_mean_item_res_by_item);
	printf("Maximum No Of Items In Residence(R) Port 3 Queue 1 :%d\n", port3_q1_max_q_depth);
	printf("Mean Time in Residence(Tr) Port 3 Queue 1(Calculated) :%f nano sec\n", calculated_port3_q1_res_time);
	printf("Mean Time in Residence(Tr) Port 3 Queue 1(Observerd) :%f nano sec\n", port3_q1_mean_res_time);
	printf("Maximum Time In Residence(TR) Port 3 Queue 1 :%d nano sec\n", port3_q1_max_res_tm);

	printf("\n");

	//printf("Mean # items in Residence(r) Port 3 Queue 2(Observerd) :%f\n", port3_q2_mean_item_res);
	printf("Mean # items in Residence(r) Port 3 Queue 2(Calculated) :%f\n", calculated_port3_q2_res_items);
	printf("Mean # items in Residence(r) Port 3 Queue 2(Observerd)  :%f\n", port3_q2_mean_item_res_by_item);
	printf("Maximum No Of Items In Residence(R) Port 3 Queue 2 :%d\n", port3_q2_max_q_depth);
	printf("Mean Time in Residence(Tr) Port 3 Queue 2(Calculated) :%f nano sec\n", calculated_port3_q2_res_time);
	printf("Mean Time in Residence(Tr) Port 3 Queue 2(Observerd) :%f nano sec\n", port3_q2_mean_res_time);
	printf("Maximum Time In Residence(TR) Port 3 Queue 2 :%d nano sec\n", port3_q2_max_res_tm);


	printf("\n\n=================================================================================================\n\n");
	
	
	
}


void calculate_mean_times()
{
	float Ts11, Ts12, Ts21, Ts22, Ts31, Ts32;
	float rho11, rho12, rho21, rho22, rho31, rho32;
	float q11_mean_res_time, q12_mean_res_time, q21_mean_res_time, q22_mean_res_time, q31_mean_res_time, q32_mean_res_time;
	float q11_mean_res_items, q12_mean_res_items, q21_mean_res_items, q22_mean_res_items, q31_mean_res_items, q32_mean_res_items;

	 Ts11 = (float)((float)INPUT_LINK1_QUEUE1_MEAN_PKT_SIZE) * 8 / ((float)OUTPUT_LINK1_QUEUE1_SPEED); /* Calculating mean service time for Queue1-1 */
	    Ts12 = (float)((float)INPUT_LINK1_QUEUE2_MEAN_PKT_SIZE) * 8 / ((float)OUTPUT_LINK1_QUEUE2_SPEED); /* Calculating mean service time for Queue1-2 */
	    Ts21 = (float)((float)INPUT_LINK2_QUEUE1_MEAN_PKT_SIZE) * 8 / ((float)OUTPUT_LINK2_QUEUE1_SPEED); /* Calculating mean service time for Queue2-1 */
	    Ts22 = (float)((float)INPUT_LINK2_QUEUE2_MEAN_PKT_SIZE) * 8 / ((float)OUTPUT_LINK2_QUEUE2_SPEED);
	    Ts31 = (float)((float)INPUT_LINK3_QUEUE1_MEAN_PKT_SIZE) * 8 / ((float)OUTPUT_LINK3_QUEUE1_SPEED);
	    Ts32 = (float)((float)INPUT_LINK3_QUEUE2_MEAN_PKT_SIZE) * 8 / ((float)OUTPUT_LINK3_QUEUE2_SPEED);

	    rho11 = INPUT_LINK1_QUEUE1_SPEED * Ts11;                /* Calculating Rho(utilization) of Queue1-1 */
	    rho12 = INPUT_LINK1_QUEUE2_SPEED * Ts12;                /* Calculating Rho(utilization) of Queue1-2 */
	    rho21 = INPUT_LINK2_QUEUE1_SPEED * Ts21;                /* Calculating Rho(utilization) of Queue2-1 */
	    rho22 = INPUT_LINK2_QUEUE2_SPEED * Ts22;
	    rho31 = INPUT_LINK3_QUEUE1_SPEED * Ts31;
	    rho32 = INPUT_LINK3_QUEUE2_SPEED * Ts32;

	    q11_mean_res_time = Ts11 / (1-rho11);        /* Calculating Mean residence time of Queue1-1 */
	       q12_mean_res_time = Ts12 / (1-rho12);        /* Calculating Mean residence time of Queue1-2 */
	       q21_mean_res_time = Ts21 / (1-rho21);         /* Calculating Mean residence time of Queue2-1 */
	       q22_mean_res_time = Ts22 / (1-rho22);        /* Calculating Mean residence time of Queue2-2 */
	       q31_mean_res_time = Ts31 / (1-rho31);
	       q32_mean_res_time = Ts32 / (1-rho32);

	       q11_mean_res_items = rho11 / (1-rho11);        /* Calculating Mean number of items in Queue1-1 */
	       q12_mean_res_items = rho12 / (1-rho12);        /* Calculating Mean number of items in Queue1-2 */
	       q21_mean_res_items = rho21 / (1-rho21);
	       q22_mean_res_items = rho22 / (1-rho22);
	       q31_mean_res_items = rho31 / (1-rho31);
	       q32_mean_res_items = rho32 / (1-rho32);

	       calculated_port1_q1_res_time = q11_mean_res_time * SECOND_TO_NANOSEC;
	       calculated_port1_q2_res_time = q12_mean_res_time * SECOND_TO_NANOSEC;
	        calculated_port2_q1_res_time = q21_mean_res_time * SECOND_TO_NANOSEC;
	        calculated_port2_q2_res_time = q22_mean_res_time * SECOND_TO_NANOSEC;
	        calculated_port3_q1_res_time = q31_mean_res_time * SECOND_TO_NANOSEC;
	        calculated_port3_q2_res_time = q32_mean_res_time * SECOND_TO_NANOSEC;


	        calculated_port1_q1_res_items = q11_mean_res_items;
	        calculated_port1_q2_res_items = q12_mean_res_items;
	        calculated_port2_q1_res_items = q21_mean_res_items;
	        calculated_port2_q2_res_items = q22_mean_res_items;
	        calculated_port3_q1_res_items = q31_mean_res_items;
	        calculated_port3_q2_res_items = q32_mean_res_items;


}

/*
void print_inputs()
{
	printf("INPUT_LINK1_QUEUE1_SPEED :%d\n", INPUT_LINK1_QUEUE1_SPEED);
	printf("INPUT_LINK1_QUEUE2_SPEED :%d\n", INPUT_LINK1_QUEUE2_SPEED);

	printf("INPUT_LINK2_QUEUE1_SPEED :%d\n", INPUT_LINK2_QUEUE1_SPEED);
	printf("INPUT_LINK2_QUEUE2_SPEED :%d\n", INPUT_LINK2_QUEUE2_SPEED);

	printf("INPUT_LINK3_QUEUE1_SPEED :%d\n", INPUT_LINK3_QUEUE1_SPEED);
	printf("INPUT_LINK3_QUEUE2_SPEED :%d\n", INPUT_LINK3_QUEUE2_SPEED);

	printf("INPUT_LINK1_QUEUE1_MEAN_PKT_SIZE :%d\n", INPUT_LINK1_QUEUE1_MEAN_PKT_SIZE);
	printf("INPUT_LINK1_QUEUE2_MEAN_PKT_SIZE :%d\n", INPUT_LINK1_QUEUE2_MEAN_PKT_SIZE);

	printf("INPUT_LINK2_QUEUE1_MEAN_PKT_SIZE :%d\n", INPUT_LINK2_QUEUE1_MEAN_PKT_SIZE);
	printf("INPUT_LINK2_QUEUE2_MEAN_PKT_SIZE :%d\n", INPUT_LINK2_QUEUE2_MEAN_PKT_SIZE);

	printf("INPUT_LINK3_QUEUE1_MEAN_PKT_SIZE :%d\n", INPUT_LINK1_QUEUE1_MEAN_PKT_SIZE);
	printf("INPUT_LINK3_QUEUE2_MEAN_PKT_SIZE :%d\n", INPUT_LINK1_QUEUE2_MEAN_PKT_SIZE);

	printf("OUTPUT_LINK1_QUEUE1_SPEED :%d\n", OUTPUT_LINK1_QUEUE1_SPEED);
	printf("OUTPUT_LINK1_QUEUE2_SPEED :%d\n", OUTPUT_LINK1_QUEUE2_SPEED);

	printf("OUTPUT_LINK2_QUEUE1_SPEED :%d\n", OUTPUT_LINK2_QUEUE1_SPEED);
	printf("OUTPUT_LINK2_QUEUE2_SPEED :%d\n", OUTPUT_LINK2_QUEUE2_SPEED);

	printf("OUTPUT_LINK3_QUEUE1_SPEED :%d\n", OUTPUT_LINK3_QUEUE1_SPEED);
	printf("OUTPUT_LINK3_QUEUE2_SPEED :%d\n", OUTPUT_LINK3_QUEUE2_SPEED);

	printf("Input File Names: ");
	printf("INPUT_FILE1_NAME %s INPUT_FILE2_NAME %s INPUT_FILE2_NAME %s\n", INPUT_FILE1_NAME, INPUT_FILE2_NAME, INPUT_FILE3_NAME);

	printf("IPV4 Routing Table File Names:\nn ");
	printf("%s", IPV4_ROUTING_TBL_NAME);

	printf("\nOutput File Names: ");
	printf("%s %s %s", IPV4_OUTPUT_FILE1_NAME, IPV4_OUTPUT_FILE2_NAME, IPV4_OUTPUT_FILE3_NAME);


}
*/
/*

void get_user_inputs()
{
	printf("\n=========================================================================\n");
	printf("                         WELCOME TO SIMULATOR                             \n");
	printf("\n=========================================================================\n");
	printf("\nPLease provide the following inputs to start the simulation...\n");

	printf("\nArrival Rate For queues Port1_q1, Port1_q2 (Packets/sec):\n ");
//	scanf("%d", &INPUT_LINK1_QUEUE1_SPEED);

	scanf("%d %d", &INPUT_LINK1_QUEUE1_SPEED, &INPUT_LINK1_QUEUE2_SPEED);
	printf("\nMean Packet size queues Port1_q1, Port1_q2 (in Octets):\n ");
	scanf("%d %d", &INPUT_LINK1_QUEUE1_MEAN_PKT_SIZE, &INPUT_LINK1_QUEUE2_MEAN_PKT_SIZE);
	printf("\nOutput Rate Port1_q1, Port1_q2 (bits/sec):\n ");
	scanf("%d %d", &OUTPUT_LINK1_QUEUE1_SPEED, &OUTPUT_LINK1_QUEUE2_SPEED);

	printf("\nArrival Rate For queues Port2_q1, Port2_q2 (Packets/sec):\n ");
	scanf("%d %d", &INPUT_LINK2_QUEUE1_SPEED, &INPUT_LINK2_QUEUE2_SPEED);
	printf("\nMean Packet size queues Port2_q1, Port2_q2 (in Octets):\n ");
	scanf("%d %d", &INPUT_LINK2_QUEUE1_MEAN_PKT_SIZE, &INPUT_LINK2_QUEUE2_MEAN_PKT_SIZE);
	printf("\nOutput Rate Port2_q1, Port2_q2 (bits/sec):\n ");
	scanf("%d %d", &OUTPUT_LINK2_QUEUE1_SPEED, &OUTPUT_LINK2_QUEUE2_SPEED);

	printf("\nArrival Rate For queues Port3_q1, Port3_q2 (Packets/sec):\n ");
	scanf("%d %d", &INPUT_LINK3_QUEUE1_SPEED, &INPUT_LINK3_QUEUE2_SPEED);
	printf("\nMean Packet size queues Port3_q1, Port3_q2 (in Octets):\n ");
	scanf("%d %d", &INPUT_LINK3_QUEUE1_MEAN_PKT_SIZE, &INPUT_LINK3_QUEUE2_MEAN_PKT_SIZE);
	printf("\nOutput Rate Port3_q1, Port3_q2 (bits/sec):\n ");
	scanf("%d %d", &OUTPUT_LINK3_QUEUE1_SPEED, &OUTPUT_LINK3_QUEUE2_SPEED);

	printf("\nInput File Names:\n ");
	scanf("%s %s %s", INPUT_FILE1_NAME, INPUT_FILE2_NAME, INPUT_FILE3_NAME);

	printf("\nIPV4 Routing Table File Name:\n ");
	scanf("%s", IPV4_ROUTING_TBL_NAME);

	printf("\nOutput File Names:\n ");
	scanf("%s %s %s", IPV4_OUTPUT_FILE1_NAME, IPV4_OUTPUT_FILE2_NAME, IPV4_OUTPUT_FILE3_NAME);

//	print_inputs();

//	sleep(5);


}

*/

int main(void) {

	int in_thread_status = FALSE, out_thread_status = FALSE;

//Open the logfile for logging messages
	logfile  = fopen("log.txt", "w");
	if(NULL == logfile)
	{
		printf("No Disk space for logging, Exiting Simulator...\n");
		exit(0);

	}

	//Open the logfile for logging messages
		route  = fopen("table", "w");
		if(NULL == route)
		{
			printf("No Disk space for logging, Exiting Simulator...\n");
			exit(0);

		}
//	get_user_inputs();

		calculate_mean_times();

	//calculate_threads_slp_times();

//Read  3 files and store thr content in array
	Populate_Buffers();
	

//	Populate_Routing_Tbls();
	Populate_Routing_Tbls(IPV4_ROUTING_TBL_NAME, IPV6_ROUTING_TBL_NAME);
	
//	Print_Routing_Tbls();

	//sleep(2);

	initialize_output_files();
	
	process_link1_packets();
	process_link2_packets();
	process_link3_packets();

	printf("Starting Input Threads\n");
	fprintf(logfile, "Starting Input Threads\n");

	in_thread_status = start_input_threads();
	if (FALSE == in_thread_status)
	{
		printf("Could Not Start The Input Threads \n");
		fprintf(logfile, "Could Not Start The Input Threads \n");
	}

//Start Output Threads
	printf("Starting Output Threads\n");
	out_thread_status = start_output_link_threads();
	if (FALSE == out_thread_status)
	{
		printf("Could Not Start The Output Threads \n");
		fprintf(logfile, "Could Not Start The Output Threads \n");
	}
	
	pthread_join( input_thread1, NULL);
	pthread_join( input_thread2, NULL);
	pthread_join( input_thread3, NULL);
	pthread_join( input_thread4, NULL);
	pthread_join( input_thread5, NULL);
	pthread_join( input_thread6, NULL);

	printf("All Input Threads Finished ...\n");
	
	//As input_thread1 has finished signal the corresponding output thread to exit
	sem_post(&ipv4_port1_sem);
	

	//As input_thread2 has finished signal the corresponding output thread to exit
	sem_post(&ipv4_port2_sem);
	

	//As input_thread3 has finished signal the corresponding output thread to exit
	sem_post(&ipv4_port3_sem);



	//Main Thread waiting for all output threads to exit before exiting
	pthread_join( output_thread1, NULL);
	printf("output_thread1 Finished ...\n");

	pthread_join( output_thread2, NULL);
	printf("output_thread2 Finished ...\n");

	pthread_join( output_thread3, NULL);
	printf("output_thread3 Finished ...\n");
	
	write_Packets_To_Files();

	printf("All Threads Finished ...Preparing Report\n");
	
	
//	fclose(pkt);
	make_report();
//	Print_Routing_Tbls();

	return 0;
}

