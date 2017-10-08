/**
* @file     project-conf.h
* @brief    Contains the configuration for project
* @version  1.0
* @date     20 May. 2017
* @author   Tiago Costa & Ricardo Jesus & Claudio Prates
*
**/

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/** @addtogroup Configurations
 * @{
 */
// Radio
#define IEEE802154_CONF_PANID            0xABCD
#define RF_CORE_CONF_CHANNEL                 25

// TODO just to test
#define NETSTACK_CONF_RDC                nullrdc_driver

#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW          60
#endif

// TODO: apagar?
#ifndef WEBSERVER_CONF_CFS_CONNS
#define WEBSERVER_CONF_CFS_CONNS 2
#endif

/* Increase rpl-border-router IP-buffer when using more than 64. */
#undef REST_MAX_CHUNK_SIZE
#define REST_MAX_CHUNK_SIZE            48


/* Multiplies with chunk size, be aware of memory constraints. */
#undef COAP_MAX_OPEN_TRANSACTIONS
#define COAP_MAX_OPEN_TRANSACTIONS     4

/* Filtering .well-known/core per query can be disabled to save space. */
#undef COAP_LINK_FORMAT_FILTERING
#define COAP_LINK_FORMAT_FILTERING     0
#undef COAP_PROXY_OPTION_PROCESSING
#define COAP_PROXY_OPTION_PROCESSING   0

/* Disable client-side support for COAP observe to save RAM */
#define COAP_OBSERVE_CLIENT 						0

/* Maximum number of online nodes */
#define NODE_TABLE_CONF_SIZE                        5


#undef UIP_CONF_TCP
#define UIP_CONF_TCP                   				1

#define UIP_CONF_IPV6_RPL							0
#define NETSTACK_CONF_WITH_IPV6						1
#define UIP_CONF_ND6_SEND_NA						1
#define UIP_CONF_ROUTER								1 /* TODO: Verificar se dá para passar a zero */
#define UIP_CONF_UDP								1
// Multi Interfaces
#define UIP_CONF_DS6_INTERFACES_NUMBER				2
#define UIP_CONF_RADIO_INTERFACE_ID                 0 /* Radio interface's id */
#define UIP_CONF_DEFAULT_INTERFACE_ID               1 /* Ethernet interface's id */
#define UIP_CONF_DS6_DEFAULT_PREFIX					0xFEC0
// Utilizado para construir o endereço Site-Local do nó
#define IPV6_CONF_ADDR_8			                0xA
// Used to make eth ll address different from radio interface
#define IP_LINK_LOCAL_PREFIX_BYTE	                (0x21)
// Link Layer header size - Ethernet - 14
#define UIP_CONF_LLH_LEN							14

// To save RAM
#define UIP_CONF_MAX_ROUTES                         0
#define NBR_TABLE_CONF_MAX_NEIGHBORS                10
#define PROCESS_CONF_NO_PROCESS_NAMES               1
//
#define UIP_CONF_IPV6_QUEUE_PKT  1 /*!< Enables Queue packet buffer */
#define QUEUEBUF_CONF_NUM  4 /*!< Limits the Queue buffer to 4 packets at a time. Uses ~830 bytes of RAM */


// Disable link statistics to avoid nbr-table bug
#define LINK_STATS_CONF_ENABLED                     0
// FIXME: This temporarely disables CSMA Driver to avoid the bug below
// https://github.com/contiki-os/contiki/pull/2161
#define NETSTACK_CONF_MAC                           nullmac_driver

// TODO: clean!
// Used to have an user handler for ICMP packets - core/net/tcpip.c icmp6_new()
// 	- Associates the process
// /core/net/ipv6/uip6.c - uip_process()
//	- UIP_ICMP6_APPCALL() - call the handler
//#define UIP_CONF_ICMP6								1	// 0 as default
//#define UIP_CONF_IPV6_QUEUE_PKT 					1 // ALTERAR NO CONTIKI-CONF DA PLATAFORMA - 0 por defeito
//#define UIP_CONF_ROUTER							0 // defenido em platform/contiki-conf.h
//#define UIP_CONF_TCP								1
//#define UIP_CONF_ND6_SEND_RA						1 // defenido em platform/contiki-conf.h
//#define UIP_CONF_ND6_SEND_RS						1 // defenido em platform/contiki-conf.h



/**
 * Está aqui para resolver o problema do Hard Fault por desalinhamento na execução de store/load multiplos.
 * uip_ip6addr_copy é utilizada nos sockets HTTP
 */
#define uip_ipaddr_copy(dest, src) 		memcpy(dest, src, sizeof(uip_ip6addr_t));
#define uip_ip6addr_copy(dest, src)		memcpy(dest, src, sizeof(uip_ip6addr_t));

/**
 * @}
 */

#endif
