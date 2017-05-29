/*
 * node.c
 *
 */

#include "coap_node.h"

coap_node_entry_t online_coap_nodes_list[CONTROLLER_MAX_ONLINE_COAP_NODES];

/*---------------------------------------------------------------------------*/
uint8_t get_coap_node(uint32_t hash, coap_node_entry_t **node_ptr) {
	int i;
	for(i = 0; i < sizeof(online_coap_nodes_list); i++) {
		if(hash == online_coap_nodes_list[i].hash) {
			*node_ptr = &online_coap_nodes_list[i];
			return 1;
		}
	}

	return 0;
}
/*---------------------------------------------------------------------------*/

