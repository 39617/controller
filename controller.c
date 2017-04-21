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
  res_mirror,
  res_chunks,
  res_separate,
  res_push,
  res_event,
  res_sub,
  res_b1_sep_b2,
  res_http_index;

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

/* Begin Post Handlers */
static int
defaults_post_handler(char *key, int key_len, char *val, int val_len)
{
	PRINTF("key = %s | val = %s\n", key, val);
	return 1;
}

HTTPD_SIMPLE_POST_HANDLER(defaults, defaults_post_handler);
/* End Post Handlers */



/*---------------------------------------------------------------------------*/
PROCESS_THREAD(controller_process, ev, data)
{
	PROCESS_BEGIN();
	PROCESS_PAUSE();
	httpd_simple_register_post_handler(&defaults_handler);

	static struct etimer et;

	uip_ds6_select_netif(UIP_DEFAULT_INTERFACE_ID);

	print_local_addresses();

	/* Initialize the REST engine. */
	rest_init_engine();

	/*
	* Bind the resources to their Uri-Path.
	* WARNING: Activating twice only means alternate path, not two instances!
	* All static variables are the same for each URI path.
	*/
	rest_activate_resource(&res_hello, "test/hello");
	/*  rest_activate_resource(&res_mirror, "debug/mirror"); */
	/*  rest_activate_resource(&res_chunks, "test/chunks"); */
	/*  rest_activate_resource(&res_separate, "test/separate"); */
	rest_activate_resource(&res_push, "test/push");
	/*  rest_activate_resource(&res_event, "sensors/button"); */
	/*  rest_activate_resource(&res_sub, "test/sub"); */
	/*  rest_activate_resource(&res_b1_sep_b2, "test/b1sepb2"); */

	rest_activate_resource(&res_http_index, "/index");



	while(1) {
		etimer_set(&et, 3*CLOCK_SECOND);
		PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

		PRINTF("\nBump!\n");
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
