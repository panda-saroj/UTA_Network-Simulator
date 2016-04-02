/*
 * simulator_hash_inputs.h
 *
 *  Created on: Mar 23, 2016
 *      Author: saroj
 */

#ifndef SIMULATOR_HASH_INPUTS_H_
#define SIMULATOR_HASH_INPUTS_H_


//Input Rates for all queues

//Link 1
#define INPUT_LINK1_QUEUE1_SPEED 30 //(Lambda1_q1)
#define INPUT_LINK1_QUEUE2_SPEED 35 //(Lambda1_q2)

//Link 2
#define INPUT_LINK2_QUEUE1_SPEED 40 //(Lambda2_q1)
#define INPUT_LINK2_QUEUE2_SPEED 45 //(Lambda2_q2)

//Link 3
#define INPUT_LINK3_QUEUE1_SPEED 20 //(Lambda3_q1)
#define INPUT_LINK3_QUEUE2_SPEED 25 //(Lambda3_q2)

//Mean Packet Size for all queues

//Link 1
#define INPUT_LINK1_QUEUE1_MEAN_PKT_SIZE 500
#define INPUT_LINK1_QUEUE2_MEAN_PKT_SIZE 500

//Link 2
#define INPUT_LINK2_QUEUE1_MEAN_PKT_SIZE 500
#define INPUT_LINK2_QUEUE2_MEAN_PKT_SIZE 500

//Link 3
#define INPUT_LINK3_QUEUE1_MEAN_PKT_SIZE 500
#define INPUT_LINK3_QUEUE2_MEAN_PKT_SIZE 500

//Output Rates For All Queues

//Link 1
#define OUTPUT_LINK1_QUEUE1_SPEED 56000000 //Ts1_q1
#define OUTPUT_LINK1_QUEUE2_SPEED 56000000 //Ts1_q2

//Link 2
#define OUTPUT_LINK2_QUEUE1_SPEED 56000000 //Ts2_q1
#define OUTPUT_LINK2_QUEUE2_SPEED 56000000 //Ts2_q2

//Link 3
#define OUTPUT_LINK3_QUEUE1_SPEED 56000000 //Ts3_q1
#define OUTPUT_LINK3_QUEUE2_SPEED 56000000 //Ts3_q2

//Input File Names

#define INPUT_FILE1_NAME "Ph2Link1_MM1"
#define INPUT_FILE2_NAME "Ph2Link2_MM1"
#define INPUT_FILE3_NAME "Ph2Link3_MM1"

/*
#define INPUT_FILE1_NAME "link1rand"
#define INPUT_FILE2_NAME "link2rand"
#define INPUT_FILE3_NAME "link3rand"
*/
//Output File Names
#define IPV4_OUTPUT_FILE1_NAME "outfile1"
#define IPV4_OUTPUT_FILE2_NAME "outfile2"
#define IPV4_OUTPUT_FILE3_NAME "outfile3"


//Forwarding or Routing Table File Names
//#define IPV4_ROUTING_TBL_NAME "ipv4Table"
//#define IPV4_ROUTING_TBL_NAME "forwardingtable1"
#define IPV4_ROUTING_TBL_NAME "Ph2ForwardingTable"

//#define IPV4_ROUTING_TBL_NAME "forwardingtable_ph2"
#define IPV6_ROUTING_TBL_NAME "ipv6Table"


#endif /* SIMULATOR_HASH_INPUTS_H_ */
