/**
 * @file     coap_node.h
 * @brief    Contains the Coap Node definitions
 * @version  1.0
 * @date     01 Jun. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#ifndef NODE_COAP_NODE_H_
#define NODE_COAP_NODE_H_

/** @defgroup CoAP CoAP
 * @{
 */

/** @addtogroup CoAP_Node
 * @{
 */

#include <stdint.h>

/*---------------------------------------------------------------------------*/
#ifndef CONTROLLER_CONF_MAX_ONLINE_COAP_NODES
#define CONTROLLER_MAX_ONLINE_COAP_NODES                   5
#else
#define CONTROLLER_MAX_ONLINE_COAP_NODES                   CONTROLLER_CONF_MAX_ONLINE_COAP_NODES
#endif
/*---------------------------------------------------------------------------*/
typedef struct _coap_node_entry
{
    uint32_t hash; /* Bases on MAC and IPv6 */
    uint8_t ip[16]; /* Node's IPv6 */
    uint8_t type; /* Node type. Helps platform or mobile app to know the right end-point */
    uint8_t requests; // number of request executing for this node
    void * node_data; // normally used to point to the http_state when passed to CoAP client process
// TODO: adicionar 8 bytes para uptime?
} coap_node_entry_t, *p_coap_node_entry_t;
/*---------------------------------------------------------------------------*/
/* List of all nodes sending keep-alives */
extern coap_node_entry_t online_coap_nodes_list[CONTROLLER_MAX_ONLINE_COAP_NODES];
/*---------------------------------------------------------------------------*/

/**
 * @brief Search for a node with 'hash'
 * @param hash : Node Hash based on MAC and IPv6
 * @param node_ptr : Pointer to the node found
 * @return uint8_t
 */
uint8_t get_coap_node(uint32_t hash, coap_node_entry_t **node_ptr);

/**
 * @}
 */

/**
 * @}
 */

#endif /* NODE_COAP_NODE_H_ */
