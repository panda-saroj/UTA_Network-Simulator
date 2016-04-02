/*
 * bld_routing_tbl.h
 *
 *  Created on: Feb 27, 2016
 *      Author: saroj
 */

#ifndef BLD_ROUTING_TBL_H_
#define BLD_ROUTING_TBL_H_

#include "cn2simulator.h"
//#include "simulator_hash_inputs.h"


void print_hex(unsigned char *ptr, int size);
void print_routing_info(ipv4RoutingInfo *info);
void print_ipv4_routing_tbl(ipv4RoutingTable *tbl);
ipv4RoutingTable * build_ipv4_info(unsigned char *raw_info);
int buildIpv4RoutingTbl();
void Populate_Routing_Tbls();
void Print_Routing_Tbls();


#endif /* BLD_ROUTING_TBL_H_ */
