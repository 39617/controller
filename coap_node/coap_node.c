/**
 * @file     coap_node.c
 * @brief    Contains the list of online nodes and the logic to find node in that list
 * @version  1.0
 * @date     01 Jun. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#include "coap_node.h"

coap_node_entry_t online_coap_nodes_list[CONTROLLER_MAX_ONLINE_COAP_NODES]; /*!< List of Online Nodes */

/**
 * @brief Gets the a Node on the List based on the hash passed
 * @param hash : Hash based on MAC and IPv6
 * @param node_ptr : Address to deposit the found node
 * @return uint8_t
 */
uint8_t get_coap_node(uint32_t hash, coap_node_entry_t **node_ptr)
{
    int i;
    for (i = 0; i < CONTROLLER_MAX_ONLINE_COAP_NODES; i++)
    {
        if (hash == online_coap_nodes_list[i].hash)
        {
            *node_ptr = &online_coap_nodes_list[i];
            return 1;
        }
    }
    return 0;
}
