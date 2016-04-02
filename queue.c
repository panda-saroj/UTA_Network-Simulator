/*
 * queue.c
 *
 *  Created on: Feb 26, 2016
 *      Author: saroj
 */


#include "queue.h"
#include <semaphore.h>

ipv4Node *ipv4_Port1_Queue1_front = NULL;
ipv4Node *ipv4_Port1_Queue1_rear = NULL;

ipv4Node *ipv4_Port1_Queue2_front = NULL;
ipv4Node *ipv4_Port1_Queue2_rear = NULL;

ipv4Node *ipv4_Port2_Queue1_front = NULL;
ipv4Node *ipv4_Port2_Queue1_rear = NULL;

ipv4Node *ipv4_Port2_Queue2_front = NULL;
ipv4Node *ipv4_Port2_Queue2_rear = NULL;

ipv4Node *ipv4_Port3_Queue1_front = NULL;
ipv4Node *ipv4_Port3_Queue1_rear = NULL;

ipv4Node *ipv4_Port3_Queue2_front = NULL;
ipv4Node *ipv4_Port3_Queue2_rear = NULL;

//Variables To Hold Current Max Queue Depth
int port1_q1_max_q_depth = 0, port1_q2_max_q_depth = 0;
int port2_q1_max_q_depth = 0, port2_q2_max_q_depth = 0;
int port3_q1_max_q_depth = 0, port3_q2_max_q_depth = 0;


int port1_q1_total_res_items =0, port1_q2_total_res_items = 0;
int port2_q1_total_res_items =0, port2_q2_total_res_items = 0;
int port3_q1_total_res_items =0, port3_q2_total_res_items = 0;

/*
struct timeval out_q1_1st_pkt_arrival_time;
struct timeval out_q1_last_pkt_depart_time;

struct timeval out_q2_1st_pkt_arrival_time;
struct timeval out_q2_last_pkt_depart_time;

struct timeval out_q3_1st_pkt_arrival_time;
struct timeval out_q3_last_pkt_depart_time;
*/

struct timespec out_q1_1st_pkt_arrival_time;
struct timespec out_q1_last_pkt_depart_time;

struct timespec out_q2_1st_pkt_arrival_time;
struct timespec out_q2_last_pkt_depart_time;

struct timespec out_q3_1st_pkt_arrival_time;
struct timespec out_q3_last_pkt_depart_time;

//Variables to keep track of Packets added to each queue
int port1_q1_pkt_counter = 0;
int port1_q2_pkt_counter = 0;
int port2_q1_pkt_counter = 0;
int port2_q2_pkt_counter = 0;
int port3_q1_pkt_counter = 0;
int port3_q2_pkt_counter = 0;


void addToQueue(ipv4Node **q_front, ipv4Node **q_rear, ipv4Node *node)
{
	ipv4Node *front = *q_front;
	ipv4Node *rear = *q_rear;

	if(front == NULL && rear == NULL)
	{

		front = rear = node;
		return;
	}

	rear->next = node;
	rear = node;
}

// To Enqueue an integer
void ipv4Enqueue(ipv4_port_no port_no, ipv4_queue_no q_no, ipv4Packet *ipv4Pkt)
{
	ipv4Node *node = (ipv4Node *)malloc(sizeof(ipv4Node));
	node->ipv4Pkt = ipv4Pkt;

	node->next = NULL;

	//Add Packet Arrival Time
	clock_gettime(CLOCK_REALTIME, &(ipv4Pkt->arrival_time));


	switch(port_no)
	{
	case IPV4_PORT_1:
		if (IPV4_QUEUE_1 == q_no)
		{
			if(ipv4_Port1_Queue1_front == NULL && ipv4_Port1_Queue1_rear == NULL)
			{

				ipv4_Port1_Queue1_front = ipv4_Port1_Queue1_rear = node;
				return;
			}

			ipv4_Port1_Queue1_rear->next = node;
			ipv4_Port1_Queue1_rear = node;

			//Add The Packet to Port 1 Queue 1
//			addToQueue(&ipv4_Port1_Queue1_front, &ipv4_Port1_Queue1_rear, temp);

		}
		else if (IPV4_QUEUE_2 == q_no)
		{

			if(ipv4_Port1_Queue2_front == NULL && ipv4_Port1_Queue2_rear == NULL)
			{

				ipv4_Port1_Queue2_front = ipv4_Port1_Queue2_rear = node;
				return;
			}

			ipv4_Port1_Queue2_rear->next = node;
			ipv4_Port1_Queue2_rear = node;

			//Add The Packet to Port 1 Queue 1
	//		addToQueue(&ipv4_Port1_Queue2_front, &ipv4_Port1_Queue2_rear, temp);

		}

		break;

	case IPV4_PORT_2:
		if (IPV4_QUEUE_1 == q_no)
		{

			if(ipv4_Port2_Queue1_front == NULL && ipv4_Port2_Queue1_rear == NULL)
			{

				ipv4_Port2_Queue1_front = ipv4_Port2_Queue1_rear = node;
				return;
			}

			ipv4_Port2_Queue1_rear->next = node;
			ipv4_Port2_Queue1_rear = node;

			//Add The Packet to Port 1 Queue 1
			//addToQueue(&ipv4_Port2_Queue1_front, &ipv4_Port2_Queue1_rear, temp);

		}
		else if (IPV4_QUEUE_2 == q_no)
		{

			if(ipv4_Port2_Queue2_front == NULL && ipv4_Port2_Queue2_rear == NULL)
			{

				ipv4_Port2_Queue2_front = ipv4_Port2_Queue2_rear = node;
				return;
			}

			ipv4_Port2_Queue2_rear->next = node;
			ipv4_Port2_Queue2_rear = node;

			//Add The Packet to Port 1 Queue 1
			//addToQueue(&ipv4_Port2_Queue2_front, &ipv4_Port2_Queue2_rear, temp);

		}

		break;

	case IPV4_PORT_3:
		if (IPV4_QUEUE_1 == q_no)
		{

			if(ipv4_Port3_Queue1_front == NULL && ipv4_Port3_Queue1_rear == NULL)
			{

				ipv4_Port3_Queue1_front = ipv4_Port3_Queue1_rear = node;
				return;
			}

			ipv4_Port3_Queue1_rear->next = node;
			ipv4_Port3_Queue1_rear = node;

			//Add The Packet to Port 1 Queue 1
			//addToQueue(&ipv4_Port3_Queue1_front, &ipv4_Port3_Queue1_rear, temp);

		}
		else if (IPV4_QUEUE_2 == q_no)
		{

			if(ipv4_Port3_Queue2_front == NULL && ipv4_Port3_Queue2_rear == NULL)
			{

				ipv4_Port3_Queue2_front = ipv4_Port3_Queue2_rear = node;
				return;
			}

			ipv4_Port3_Queue2_rear->next = node;
			ipv4_Port3_Queue2_rear = node;

			//Add The Packet to Port 1 Queue 1
			//addToQueue(&ipv4_Port3_Queue2_front, &ipv4_Port3_Queue2_rear, temp);

		}

		break;
	}

}

ipv4Packet * removeFromQueue(ipv4Node **q_front, ipv4Node **q_rear, ipv4_port_no port_no, ipv4_queue_no q_no)
{
	printf("Inside removeFromQueue Port %d Queue %d\n", port_no, q_no);
	ipv4Node *node = NULL;
	ipv4Packet *ipv4Pkt = NULL;
	ipv4Node *front = *q_front;

	ipv4Node *rear = *q_rear;


	node = front; //Get the Front Item from the queue

	if (front == rear) //Last Packet in the Queue
	{
		front = rear = NULL;

	}
	else
	{
		front = front->next;
	}

	ipv4Pkt = node->ipv4Pkt;
	free(node);
//	printf("Exiting removeFromQueue Port %d Queue %d\n", port_no, q_no);
	return ipv4Pkt;

}

ipv4Packet *  ipv4Dequeue(ipv4_port_no port_no, ipv4_queue_no q_no)
{


	ipv4Packet *ipv4Pkt = NULL;

	ipv4Node *first_node = NULL;
	ipv4Node *last_node = NULL;

	int current_q_depth = 0;


	switch(port_no)
	{
	case IPV4_PORT_1:
		if (IPV4_QUEUE_1 == q_no)
		{
			//If Queue Empty, return NULL Immediately
			if (NULL == ipv4_Port1_Queue1_front)
				return NULL;
			/*
			 * Logic to Check the current Queue Depth, if queue Not empty
			 */
			first_node = ipv4_Port1_Queue1_front;


			if(NULL != ipv4_Port1_Queue1_front)
			{
				last_node = ipv4_Port1_Queue1_rear;
				current_q_depth = (last_node->ipv4Pkt->pkt_counter - first_node->ipv4Pkt->pkt_counter) +1;

				//Add the Depth to Total Resident Items
				port1_q1_total_res_items += current_q_depth;

				if (port1_q1_max_q_depth < current_q_depth)
				{
					//Update the Queue Depth
					port1_q1_max_q_depth = current_q_depth;
				}

				if (ipv4_Port1_Queue1_front == ipv4_Port1_Queue1_rear)
				{

					ipv4_Port1_Queue1_front = ipv4_Port1_Queue1_rear = NULL;
				}
				else
				{
					ipv4_Port1_Queue1_front = ipv4_Port1_Queue1_front->next;
				}

			}
/*			else
				printf("IPV4_PORT_1 IPV4_QUEUE_1 empty\n");
*/
		}

		else if (IPV4_QUEUE_2 == q_no)
		{
			//If Queue Empty, return NULL Immediately
			if (NULL == ipv4_Port1_Queue2_front)
				return NULL;
			/*
			 * Logic to Check the current Queue Depth, if queue Not empty
			 */
			first_node = ipv4_Port1_Queue2_front;

			if(NULL != ipv4_Port1_Queue2_front)
			{
				last_node = ipv4_Port1_Queue2_rear;

				current_q_depth = (last_node->ipv4Pkt->pkt_counter - first_node->ipv4Pkt->pkt_counter) +1;

				//Add the Depth to Total Resident Items
				port1_q2_total_res_items += current_q_depth;

				if (port1_q2_max_q_depth < current_q_depth)
				{
					//Update the Queue Depth
					port1_q2_max_q_depth = current_q_depth;
				}

				if (ipv4_Port1_Queue2_front == ipv4_Port1_Queue2_rear)
				{

					ipv4_Port1_Queue2_front = ipv4_Port1_Queue2_rear = NULL;
				}
				else
				{
					ipv4_Port1_Queue2_front = ipv4_Port1_Queue2_front->next;
				}

			}
/*			else
				printf("IPV4_PORT_1 IPV4_QUEUE_2 empty\n");
*/
		}

		break;

	case IPV4_PORT_2:
		if (IPV4_QUEUE_1 == q_no)
		{
			//If Queue Empty, return NULL Immediately
			if (NULL == ipv4_Port2_Queue1_front)
				return NULL;

			/*
			 * Logic to Check the current Queue Depth, if queue Not empty
			 */
			first_node = ipv4_Port2_Queue1_front;

			if(NULL != ipv4_Port2_Queue1_front)
			{
				last_node = ipv4_Port2_Queue1_rear;

				current_q_depth = (last_node->ipv4Pkt->pkt_counter - first_node->ipv4Pkt->pkt_counter) +1;

				//Add the Depth to Total Resident Items
				port2_q1_total_res_items += current_q_depth;

				if (port2_q1_max_q_depth < current_q_depth)
				{
					//Update the Queue Depth
					port2_q1_max_q_depth = current_q_depth;
				}

				if (ipv4_Port2_Queue1_front == ipv4_Port2_Queue1_rear)
				{

					ipv4_Port2_Queue1_front = ipv4_Port2_Queue1_rear = NULL;
				}
				else
				{
					ipv4_Port2_Queue1_front = ipv4_Port2_Queue1_front->next;
				}


//Now Remove One Item from The Queue
//				ipv4Pkt = removeFromQueue(&ipv4_Port2_Queue1_front, &ipv4_Port2_Queue1_rear, port_no, q_no);
			}
/*			else
				printf("IPV4_PORT_2 IPV4_QUEUE_1 empty\n");
*/
		}

		else if (IPV4_QUEUE_2 == q_no)
		{
			//If Queue Empty, return NULL Immediately
			if (NULL == ipv4_Port2_Queue2_front)
				return NULL;

			/*
			 * Logic to Check the current Queue Depth, if queue Not empty
			 */
			first_node = ipv4_Port2_Queue2_front;

			if(NULL != ipv4_Port2_Queue2_front)
			{
				last_node = ipv4_Port2_Queue2_rear;

				current_q_depth = (last_node->ipv4Pkt->pkt_counter - first_node->ipv4Pkt->pkt_counter) +1;

				//Add the Depth to Total Resident Items
				port2_q2_total_res_items += current_q_depth;

				if (port2_q2_max_q_depth < current_q_depth)
				{
					//Update the Queue Depth
					port2_q2_max_q_depth = current_q_depth;
				}

				if (ipv4_Port2_Queue2_front == ipv4_Port2_Queue2_rear)
				{

					ipv4_Port2_Queue2_front = ipv4_Port2_Queue2_rear = NULL;
				}
				else
				{
					ipv4_Port2_Queue2_front = ipv4_Port2_Queue2_front->next;
				}

//Now Remove One Item from The Queue
//				ipv4Pkt = removeFromQueue(&ipv4_Port2_Queue2_front, &ipv4_Port2_Queue2_rear, port_no, q_no);
			}
/*			else
				printf("IPV4_PORT_2 IPV4_QUEUE_2 empty\n");
*/
		}

		break;

	case IPV4_PORT_3:

		if (IPV4_QUEUE_1 == q_no)
		{
			//If Queue Empty, return NULL Immediately
			if (NULL == ipv4_Port3_Queue1_front)
				return NULL;
			/*
			 * Logic to Check the current Queue Depth, if queue Not empty
			 */
			first_node = ipv4_Port3_Queue1_front;

			if(NULL != ipv4_Port3_Queue1_front)
			{
				last_node = ipv4_Port3_Queue1_rear;

				current_q_depth = (last_node->ipv4Pkt->pkt_counter - first_node->ipv4Pkt->pkt_counter) +1;

				//Add the Depth to Total Resident Items
				port3_q1_total_res_items += current_q_depth;

				if (port3_q1_max_q_depth < current_q_depth)
				{
					//Update the Queue Depth
					port3_q1_max_q_depth = current_q_depth;
				}

				if (ipv4_Port3_Queue1_front == ipv4_Port3_Queue1_rear)
				{

					ipv4_Port3_Queue1_front = ipv4_Port3_Queue1_rear = NULL;
				}
				else
				{
					ipv4_Port3_Queue1_front = ipv4_Port3_Queue1_front->next;
				}


//Now Remove One Item from The Queue
//				ipv4Pkt = removeFromQueue(&ipv4_Port3_Queue1_front, &ipv4_Port3_Queue1_rear, port_no, q_no);
			}
/*			else
				printf("IPV4_PORT_3 IPV4_QUEUE_1 empty\n");
*/
		}

		else if (IPV4_QUEUE_2 == q_no)
		{
			//If Queue Empty, return NULL Immediately
			if (NULL == ipv4_Port3_Queue2_front)
				return NULL;
			/*
			 * Logic to Check the current Queue Depth, if queue Not empty
			 */
			first_node = ipv4_Port3_Queue2_front;

			if(NULL != ipv4_Port3_Queue2_front)
			{
				last_node = ipv4_Port3_Queue2_rear;

				current_q_depth = (last_node->ipv4Pkt->pkt_counter - first_node->ipv4Pkt->pkt_counter) +1;

				//Add the Depth to Total Resident Items
				port3_q2_total_res_items += current_q_depth;

				if (port3_q2_max_q_depth < current_q_depth)
				{
					//Update the Queue Depth
					port3_q2_max_q_depth = current_q_depth;
				}

				if (ipv4_Port3_Queue2_front == ipv4_Port3_Queue2_rear)
				{

					ipv4_Port3_Queue2_front = ipv4_Port3_Queue2_rear = NULL;
				}
				else
				{
					ipv4_Port3_Queue2_front = ipv4_Port3_Queue2_front->next;
				}


//Now Remove One Item from The Queue
//				ipv4Pkt = removeFromQueue(&ipv4_Port3_Queue2_front, &ipv4_Port3_Queue2_rear, port_no, q_no);
			}
/*			else
				printf("IPV4_PORT_3 IPV4_QUEUE_3 empty\n");
*/
		}

		break;
	}

	//Extract The Ipv4 Packet from the Node dequeued and then free the node
	ipv4Pkt = first_node->ipv4Pkt;

	free(first_node);

	return ipv4Pkt;
}
