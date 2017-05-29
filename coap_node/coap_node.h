/*
 * node.h
 *
 */

#ifndef NODE_COAP_NODE_H_
#define NODE_COAP_NODE_H_

#include <stdint.h>


/*---------------------------------------------------------------------------*/
#ifndef CONTROLLER_CONF_MAX_ONLINE_COAP_NODES
#define CONTROLLER_MAX_ONLINE_COAP_NODES                   5
#else
#define CONTROLLER_MAX_ONLINE_COAP_NODES                   CONTROLLER_CONF_MAX_ONLINE_COAP_NODES
#endif
/*---------------------------------------------------------------------------*/
typedef struct _coap_node_entry{
	/* Bases on MAC and IPv6 */
    uint32_t hash;
    /* Node's IPv6 */
    uint8_t ip[16];
    /* Node type. Helps platform or mobile app to know the right end-point */
    uint8_t type;
    // number of request executing for this node
    uint8_t requests;
    // normally used to point to the http_state when passed to CoAP client process
    void * node_data;
    // TODO: adicionar 8 bytes para uptime?
} coap_node_entry_t, * p_coap_node_entry_t;
/*---------------------------------------------------------------------------*/
/* List of all nodes sending keep-alives */
extern coap_node_entry_t online_coap_nodes_list[CONTROLLER_MAX_ONLINE_COAP_NODES];
/*---------------------------------------------------------------------------*/

/**
 * Search for a node with 'hash'
 *
 * \param hash Node's hash
 * \param node_ptr Pointer to the node found
 *
 * \return Return 1 if the hash exist in 'online_nodes_list' or NULL if not.
 */
uint8_t get_coap_node(uint32_t hash, coap_node_entry_t **node_ptr);



#endif /* NODE_COAP_NODE_H_ */
