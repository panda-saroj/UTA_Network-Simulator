/*
 * bld_routing_tbl.c
 *
 *  Created on: Feb 24, 2016
 *      Author      : saroj
 Version     :
 Copyright   : Your copyright notice
 Description : This File contains functions to Build IPV4/IPV6 Routing Table Link List from the
 	 	 	 	 Routing Table Files
 */


#include "bld_routing_tbl.h"
//#include "simulator_hash_inputs.h"

extern FILE *route;

//extern FILE *route;

//extern char IPV4_ROUTING_TBL_NAME[MAX_FILE_NAME_SIZE];

unsigned char ipv4Record[IPV4_ROUTING_RECORD_SIZE];
int ipv4_Routing_record_total =0;


ipv4RoutingTable *ipv4RoutingTbl = NULL;
ipv6RoutingTable *ipv6RoutingTbl = NULL;

void print_hex(unsigned char *ptr, int size)
{
	int i = 0;
	while (i < size)
	{
		printf("%x ", ptr[i]);
//		fprintf(route, "%x ", ptr[i]);
//		printf()
		i++;
	}


}

void print_decimal(unsigned char *ptr, int size)
{
	int i = 0;
	while (i < size)
	{
		printf("%d ", ptr[i]);
//		fprintf(route, "%d ", ptr[i]);
		i++;
	}


}


void print_routing_info(ipv4RoutingInfo *info)
{
/*	printf("src_ntwrk :Hex\n");
//	fprintf(route, "src_ntwrk :");
//	print_hex(info->src_ntwrk, 4);
//	printf("src_ntwrk :Decimal\n");
//	print_decimal(info->src_ntwrk, 4);
//	fprintf(route, "\n");


	printf("dst_ntwrk :hex\n");
//	fprintf(route, "dst_ntwrk :");
	print_hex(info->dst_ntwrk, 4);
	printf("dst_ntwrk :dec");
	print_decimal(info->dst_ntwrk, 4);
//	fprintf(route, "\n");
*/
	printf("dst_ntwrk_mask :hex\n");
//	fprintf(route, "dst_ntwrk_mask :");
	print_hex(info->dst_ntwrk_mask, 4);
	printf("\ndst_ntwrk_mask :dec\n");
	print_decimal(info->dst_ntwrk_mask, 4);
	//fprintf(route, "\n");
	
/*
	printf("nxt_hop_ip :\n");
//	fprintf(route, "nxt_hop_ip :");
	print_hex(info->nxt_hop_ip, 4);
//	printf("\n");
	print_decimal(info->nxt_hop_ip, 4);
//	fprintf(route, "\n");
	
	printf("output_port :%hhX\n", info->output_port);
//	fprintf(route, "output_port :%x\n", info->output_port);

	printf("output_port_queue :%x\n", info->output_port_queue);
//	fprintf(route, "output_port_queue :%x\n", info->output_port_queue);
	
	printf("ECN :%x\n", info->ds_ecn);
//	fprintf(route, "ECN :%x\n", info->ds_ecn);

	printf("pad :\n");
//	fprintf(route, "pad :");
	print_hex(info->pad, 3);
//	fprintf(route, "\n");
*/
	/*
	printf("dst_ntwrk :\n");
	print_hex(info->dst_ntwrk, 4);
	*/
}

void print_ipv4_routing_tbl(ipv4RoutingTable *tbl)
{
	int i = 1;
	ipv4RoutingTable *tmp = tbl;
	while (tmp !=NULL)
	{
		printf("Entry %d\n", i);
//		fprintf(route, "Entry %d\n", i);
		print_routing_info(&(tmp->info) );
		tmp = tmp->next;
		i++;
		
	}
//	fprintf(route, "Total No Of Routing Info %d\n", (i-1));


//	fclose(route);
		
}

/*
 * Create a New ipv4RoutingInfo Entry
 */
ipv4RoutingTable * build_ipv4_info(unsigned char *raw_info)
{

	ipv4RoutingTable *ipv4Info,*current;
	unsigned char *parser = NULL;

	ipv4Info = (ipv4RoutingTable *)malloc(sizeof(ipv4RoutingTable));

	parser = raw_info;
	memcpy(ipv4Info->info.src_ntwrk, parser, 4);

	parser+= (4 + PARTITION_SIZE);
	memcpy(ipv4Info->info.dst_ntwrk, parser, 4);

	parser+= (4 + PARTITION_SIZE);
	memcpy(ipv4Info->info.dst_ntwrk_mask, parser, 4);

	parser+=(4 + PARTITION_SIZE);
	memcpy(ipv4Info->info.nxt_hop_ip, parser, 4);

	parser+=(4 + PARTITION_SIZE);
	memcpy(&(ipv4Info->info.output_port), parser, 1);

	parser+=(1 + PARTITION_SIZE);
	memcpy(&(ipv4Info->info.output_port_queue), parser, 1);

	parser+=(1 + PARTITION_SIZE);
	memcpy(&(ipv4Info->info.ds_ecn), parser, 1);

	parser+=(1 + PARTITION_SIZE);
	memcpy(ipv4Info->info.pad, parser, 3);


	ipv4Info->next = NULL;

	return ipv4Info;

}



int buildIpv4RoutingTbl(char *ipv4_table_name)
{
	int pkt_count = 0;
	FILE *fptr=NULL;

	ipv4RoutingTable *new_node, *current;

//Open The Input File To Read in Binary Mode.
//	fptr = fopen(IPV4_ROUTING_TBL_NAME, "rb");
	fptr = fopen(ipv4_table_name, "rb");
	if(NULL==fptr)
	{
		printf("Could not open %s\n", ipv4_table_name);
		exit(0);
	}
	else
	{
		if (pkt_count < MAX_ROUTING_ENTRY )
		{

			while(fread(ipv4Record, sizeof(unsigned char), IPV4_ROUTING_RECORD_SIZE, fptr))
//			while(fgets(ipv4Record, IPV4_ROUTING_RECORD_SIZE, fptr))
			{

				  new_node = build_ipv4_info(ipv4Record);

				  new_node->next=NULL;

				  if(ipv4RoutingTbl == NULL)
				  {
					  ipv4RoutingTbl = new_node;
					  current = new_node;
				  }
				  else
				  {
					  current->next = new_node;
					  current = new_node;
				  }

				pkt_count++;
				
			}
		}
		else
		{
			printf("%s has more packets than defined, Ignoring extra Packets\n", ipv4_table_name);
		}
		if (0 == pkt_count)
		{
			printf("%s is empty", ipv4_table_name);
		}
		printf("Total No of Packets in %s : %d\n", ipv4_table_name, pkt_count);
//		print_routing_tbl(ipv4RoutingTbl);
	}

	return pkt_count;

}

/*
void Sort_Ipv4RoutingTbl()
{
	int i, j, k;
	ipv4RoutingTable *prev, *current, *temp;
//	ipv4RoutingInfo temp;

	unsigned int netwrk_mask_sum_prev, netwrk_mask_sum_current;

	if ((NULL == ipv4RoutingTbl) || (NULL == ipv4RoutingTbl->next))
		return;

	if (2 == ipv4_Routing_record_total)
	{
		prev = ipv4RoutingTbl;
		current = prev->next;
		temp = ipv4RoutingTbl;

		for (k=0; k<4; k++)
		{
			netwrk_mask_sum_prev+= prev->info.dst_ntwrk_mask[k];
			netwrk_mask_sum_current+= current->info.dst_ntwrk_mask[k];

		}

		if (netwrk_mask_sum_prev < netwrk_mask_sum_current)
		{
			ipv4RoutingTbl = current;
			current->next = prev;
			prev->next = NULL;

		}
		return;

	}

	prev = ipv4RoutingTbl;
	current = prev->next;
//	temp = prev;

	for(i=0; i<(ipv4_Routing_record_total - 1); i++)
		for (j=1; j<(ipv4_Routing_record_total); i++)
		{
			netwrk_mask_sum_prev = 0;
			netwrk_mask_sum_current = 0;

			for (k=0; k<4; k++)
			{
				netwrk_mask_sum_prev+= prev->info.dst_ntwrk_mask[k];
				netwrk_mask_sum_current+= current->info.dst_ntwrk_mask[k];

			}

			if (netwrk_mask_sum_prev < netwrk_mask_sum_current)
			{
				if (prev == ipv4RoutingTbl)
				{
					ipv4RoutingTbl = current;
					temp = current->next;
					current->next = prev;
					prev->next = temp;
				}
				else
				{
					temp =
				}

			}
			prev->info.
			if()
		}


}
*/

void Sort_Ipv4RoutingTbl()
{
	int i, j, k;
	ipv4RoutingTable *prev, *current;
	ipv4RoutingInfo temp;

	unsigned int netwrk_mask_sum_prev, netwrk_mask_sum_current;

	//If No Routing Table or only one entry, nothing to compare
	if ((NULL == ipv4RoutingTbl) || (NULL == ipv4RoutingTbl->next))
		return;

	prev = ipv4RoutingTbl;


	printf("Inside Sort_Ipv4RoutingTbl\n");
	printf("ipv4_Routing_record_total :%d\n", ipv4_Routing_record_total);

//	temp = prev;

	for(i=0; i<(ipv4_Routing_record_total - 1); i++)
	{

//		for (j=1; j<(ipv4_Routing_record_total); j++)

//	{

		current = prev->next;
		while(NULL != current)
		{

			netwrk_mask_sum_prev = 0;
			netwrk_mask_sum_current = 0;

			for (k=0; k<4; k++)
			{
				netwrk_mask_sum_prev+= prev->info.dst_ntwrk_mask[k];
				netwrk_mask_sum_current+= current->info.dst_ntwrk_mask[k];

			}

			if (netwrk_mask_sum_prev < netwrk_mask_sum_current)
			{
				//Swap The Values stored in Prev to temp
				memcpy(temp.src_ntwrk, prev->info.src_ntwrk, 4);
				memcpy(temp.dst_ntwrk, prev->info.dst_ntwrk, 4);
				memcpy(temp.dst_ntwrk_mask, prev->info.dst_ntwrk_mask, 4);
				memcpy(temp.nxt_hop_ip, prev->info.nxt_hop_ip, 4);
				temp.output_port = prev->info.output_port;
				temp.output_port_queue = prev->info.output_port_queue;
				temp.ds_ecn = prev->info.ds_ecn;
				memcpy(temp.pad, prev->info.pad, 3);

				//Swap The Values stored in Current to Prev
				memcpy(prev->info.src_ntwrk, current->info.src_ntwrk, 4);
				memcpy(prev->info.dst_ntwrk, current->info.dst_ntwrk, 4);
				memcpy(prev->info.dst_ntwrk_mask, current->info.dst_ntwrk_mask, 4);
				memcpy(prev->info.nxt_hop_ip, current->info.nxt_hop_ip, 4);
				prev->info.output_port = current->info.output_port;
				prev->info.output_port_queue = current->info.output_port_queue;
				prev->info.ds_ecn = current->info.ds_ecn;
				memcpy(prev->info.pad, current->info.pad, 3);

				//Swap The Values stored in Current to Prev
				memcpy(current->info.src_ntwrk, temp.src_ntwrk, 4);
				memcpy(current->info.dst_ntwrk, temp.dst_ntwrk, 4);
				memcpy(current->info.dst_ntwrk_mask, temp.dst_ntwrk_mask, 4);
				memcpy(current->info.nxt_hop_ip, temp.nxt_hop_ip, 4);
				current->info.output_port = temp.output_port;
				current->info.output_port_queue = temp.output_port_queue;
				current->info.ds_ecn = temp.ds_ecn;
				memcpy(current->info.pad, temp.pad, 3);
			}
			current = current->next;
		}
		prev = prev->next;
	}
	return;
}


//This function will read each input file till end of file is encountered
//and populate each buffer with the packets from the file.
void Populate_Routing_Tbls(char *ipv4_tbl, char *ipv6_tbl)
{

	ipv4_Routing_record_total = buildIpv4RoutingTbl(ipv4_tbl);
	Sort_Ipv4RoutingTbl();

//	Print_Routing_Tbls();
//	sleep(1000);


}




//This function will read each input file till end of file is encountered
//and populate each buffer with the packets from the file.
void Print_Routing_Tbls()
{

	print_ipv4_routing_tbl(ipv4RoutingTbl);


}

/*

int main()
{
	Populate_Routing_Tbls();
	Print_Routing_Tbls();
	return 0;
}
*/


