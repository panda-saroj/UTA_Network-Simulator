/*
 * queue.h
 *
 *  Created on: Feb 27, 2016
 *      Author: saroj
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "cn2simulator.h"


void ipv4Enqueue(ipv4_port_no port_no, ipv4_queue_no q_no, ipv4Packet *ipv4Pkt);
ipv4Packet * ipv4Dequeue(ipv4_port_no port_no, ipv4_queue_no q_no);

void ipv6Enqueue(ipv6_queue_no q_no, ipv6Packet *ipv6Pkt);
ipv6Packet * ipv6Dequeue(ipv6_queue_no q_no);

#endif /* QUEUE_H_ */
