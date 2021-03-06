/**
 * @file     controller.c
 * @brief    Main Process, that start's some process,
 * initializes some resources and keeps toggling a LED.
 * @version  1.0
 * @date     01 Jun. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#include <string.h>
#include <stdio.h>
#include <limits.h>

#include "contiki.h"
#include "contiki-net.h"
#include "contiki-lib.h"
#include "mac.h"

#include "ti-lib.h"
#include "leds.h"

#include "controller.h"
#include "rest-engine.h"
#include "er-http.h"
#include "coap_client.h"

#include "http_request.h"

#include "netctrl-server.h"
#include "node-table.h"

/** @addtogroup Debug Debug
 * @{
 */
#define DEBUG 1
#ifdef DEBUG
#include "net/ip/uip-debug.h"
#endif /* DEBUG */

#ifdef DEBUG_COOJA
static uint8_t prefix_set;	// COOJA Only
static uip_ipaddr_t prefix;// COOJA Only
#endif
/**
 * @}
 */

PROCESS(controller_process, "Controller process"); /*!< Creates the Process controller_process with name Controller process */
AUTOSTART_PROCESSES(&controller_process); /*!< Auto Start the Process previoulsy created */

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */

/** @defgroup Global Variables
 * @{
 */
extern resource_t res_coapnodes; /*!< Resources to be activated need to be imported through the extern keyword. */
extern resource_t res_getnodes;
extern resource_t res_coaptohttp;

extern uip_ipaddr_t default_neighbor_ip6_addr; /*!< Unknow Requests goes to Default neightbor */
extern uip_eth_addr ethernet_if_addr; /*!< MAC address */
/**
 * @}
 */

/** @defgroup Debug Debug
 * @{
 */

/**
 * @brief Prints the Local Address
 *
 * It will print all the IP's configured in uip_ds6_addr_t addr_list
 *
 * @return nothing
 */
static void print_local_addresses(void)
{
    int i;
    uint8_t state;

    PRINTA("Server IPv6 addresses:\n");
    for (i = 0; i < UIP_DS6_ADDR_NB; i++)
    {
        state = uip_ds6_if.addr_list[i].state;
        if (uip_ds6_if.addr_list[i].isused
                && (state == ADDR_TENTATIVE || state == ADDR_PREFERRED))
        {
            PRINTA(" ");
            uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
            PRINTA("\n");
        }
    }
}
/**
 * @}
 */


#define LIGHT_ON_DURATION   0.05
#define LIGHT_OFF_DURATION  5
static clock_time_t update_light_signal() {
  static uint8_t state;

  if(state == 0) {
    state = 1;
    leds_on(LEDS_RED);
    return LIGHT_ON_DURATION * CLOCK_SECOND;
  }

  state = 0;
  leds_off(LEDS_RED);
  return LIGHT_OFF_DURATION * CLOCK_SECOND;
}

/**
 * @brief Main Thread of the Controller
 *
 * Responsible to initialize some resources
 *
 * Start some process's ex: coap_client_process
 *
 * Will receive event ev and the pointer to data regarding the event.
 * We are using an timer to toggle a LED to keep tracking of the system
 *
 * @param controller_process : This process
 * @param ev : The event passed to this process
 * @param data : Data regarding to the event that fired
 * @return nothing
 */
PROCESS_THREAD(controller_process, ev, data)
{
    PROCESS_BEGIN();

	PROCESS_PAUSE();

	static struct etimer et_light_signal;
	static struct etimer et_node_table;

	uip_ds6_select_netif(UIP_DEFAULT_INTERFACE_ID);
	print_local_addresses();

	/* Initialize the REST engine. */
	rest_init_engine();

	/* Start Clients */
	process_start(&coap_client_process, (void *) 0);

	/*
	 * Bind the resources to their Uri-Path.
	 * WARNING: Activating twice only means alternate path, not two instances!
	 * All static variables are the same for each URI path.
	 */
	// CoAP nodes
	rest_activate_resource(&res_coapnodes, "/coapnode");
	//
	rest_activate_resource(&res_getnodes, "/getnodes");
	// CoAP-HTTP
	rest_activate_resource(&res_coaptohttp, "coaptohttp");

	process_start(&http_request_process, (void *) 0);

	/* set timers */
	etimer_set(&et_light_signal, update_light_signal());
	// When the table is empty, dont need to refresh it
	etimer_set(&et_node_table, UINT_MAX * CLOCK_SECOND);

	/* Initializes the Network Controll protocol */
	netctrl_server_init();

	while (1)
	{
		PROCESS_WAIT_EVENT();

		if(ev == tcpip_event) {
			if(uip_newdata()) {
			    PRINTF("** Receiving UDP datagram from: ");
			    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
			    PRINTF(":%u\n  Length: %u\n", uip_ntohs(UIP_UDP_BUF->srcport),
			           uip_datalen());
			    //
			    int ret = netctrl_server_handle_net_event();
			    //if(ret == NETCTRL_RESPONSE_RESULT_REG_OK || ret == NETCTRL_RESPONSE_RESULT_RENEW_OK) {
				    // Need to update the table and reset the timer
				    etimer_set(&et_node_table, node_table_refresh());
			    //}
			}
		} else if(ev == PROCESS_EVENT_TIMER && data == &et_light_signal) {
			etimer_set(&et_light_signal, update_light_signal());
		} else if(ev == PROCESS_EVENT_TIMER && data == &et_node_table) {
			PRINTF("** Refreshing table with Timer event.\n");
			etimer_set(&et_node_table, node_table_refresh());
		}
	}

	PRINTF("END Controller\n");

    PROCESS_END();
}


/** @addtogroup Debug Debug
 * @{
 */
#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

/* Some example here */
#pragma message(VAR_NAME_VALUE(NETSTACK_CONF_RADIO))
#pragma message(VAR_NAME_VALUE(NETSTACK_CONF_RDC))
#pragma message(VAR_NAME_VALUE(NETSTACK_CONF_MAC))
#pragma message(VAR_NAME_VALUE(UIP_FALLBACK_INTERFACE))
#pragma message(VAR_NAME_VALUE(UIP_CONF_ROUTER))
#pragma message(VAR_NAME_VALUE(UIP_ND6_SEND_RA))
#pragma message(VAR_NAME_VALUE(LINKADDR_CONF_SIZE))
#pragma message(VAR_NAME_VALUE(UIP_LLH_LEN))
#pragma message(VAR_NAME_VALUE(UIP_IPH_LEN))
#pragma message(VAR_NAME_VALUE(UIP_CONF_BUFFER_SIZE))
#pragma message(VAR_NAME_VALUE(BOARD_IOID_CS))
#pragma message(VAR_NAME_VALUE(BOARD_IOID_SPI_CLK_FLASH))
#pragma message(VAR_NAME_VALUE(NBR_TABLE_CONF_MAX_NEIGHBORS))
#pragma message(VAR_NAME_VALUE(UIP_LLH_LEN))
/**
 * @}
 */
