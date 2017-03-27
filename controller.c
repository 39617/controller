#include <string.h>
#include <stdio.h>

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

#define DEBUG 1
#ifdef DEBUG
#include "net/ip/uip-debug.h"
#endif /* DEBUG */

#ifdef DEBUG_COOJA
static uint8_t prefix_set;	// COOJA Only
static uip_ipaddr_t prefix;	// COOJA Only
#endif

PROCESS(controller_process, "Controller process");
AUTOSTART_PROCESSES(&controller_process);

/*
 * Resources to be activated need to be imported through the extern keyword.
 * The build system automatically compiles the resources in the corresponding sub-directory.
 */
extern resource_t
  res_hello,
  res_http_index,
  res_api;

extern uip_ipaddr_t default_neighbor_ip6_addr;
extern uip_eth_addr ethernet_if_addr;

address_table_t addr_table[50];

static void
print_local_addresses(void)
{
  int i;
  uint8_t state;

  PRINTA("Server IPv6 addresses:\n");
  for(i = 0; i < UIP_DS6_ADDR_NB; i++) {
    state = uip_ds6_if.addr_list[i].state;
    if(uip_ds6_if.addr_list[i].isused &&
       (state == ADDR_TENTATIVE || state == ADDR_PREFERRED)) {
      PRINTA(" ");
      uip_debug_ipaddr_print(&uip_ds6_if.addr_list[i].ipaddr);
      PRINTA("\n");
    }
  }
}
//2001:db8:ac10:fe01:0:0:0:1
uip_ipaddr_t coap_server = {
        .u16[0] = 0xc0fe,
        .u16[1] = 0x0,
        .u16[2] = 0x0,
        .u16[3] = 0x0,
        .u16[4] = 0x0,
        .u16[5] = 0x0,
        .u16[6] = 0x0,
        .u16[7] = 0xb00};
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(controller_process, ev, data)
{
	PROCESS_BEGIN();
	PROCESS_PAUSE();

	static struct etimer et;

	addr_table[0].hash = 1234567890;
	memcpy(&addr_table[0].mac, &ethernet_if_addr, sizeof(ethernet_if_addr));
	memcpy(&addr_table[0].ip, &coap_server, sizeof(coap_server));


	uip_ds6_select_netif(UIP_DEFAULT_INTERFACE_ID);

	print_local_addresses();

	/* Initialize the REST engine. */
	rest_init_engine();

	/* Start Clients */
	process_start(&coap_client_process, (void *)0);

	/*
	* Bind the resources to their Uri-Path.
	* WARNING: Activating twice only means alternate path, not two instances!
	* All static variables are the same for each URI path.
	*/
	rest_activate_resource(&res_hello, "test/hello");
	/*  rest_activate_resource(&res_mirror, "debug/mirror"); */
	/*  rest_activate_resource(&res_chunks, "test/chunks"); */
	/*  rest_activate_resource(&res_separate, "test/separate"); */
	//rest_activate_resource(&res_push, "test/push");
	/*  rest_activate_resource(&res_event, "sensors/button"); */
	/*  rest_activate_resource(&res_sub, "test/sub"); */
	/*  rest_activate_resource(&res_b1_sep_b2, "test/b1sepb2"); */

	rest_activate_resource(&res_http_index, "/index");
	rest_activate_resource(&res_api, "/api");



	while(1) {
		etimer_set(&et, 3*CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		//PRINTF("\nBump!\n");
		leds_toggle(LEDS_RED);
	}

	PRINTF("END Controller\n");
	PROCESS_END();
}


#define VALUE_TO_STRING(x) #x
#define VALUE(x) VALUE_TO_STRING(x)
#define VAR_NAME_VALUE(var) #var "="  VALUE(var)

/* Some example here */
#pragma message(VAR_NAME_VALUE(NETSTACK_CONF_RADIO))
#pragma message(VAR_NAME_VALUE(NETSTACK_CONF_RDC))
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




/*---------------------------------------------------------------------------*/
