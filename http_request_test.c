#include "http_request_test.h"
#include "http_request.h"

//#include <stdio.h>
//#include <string.h>

//#if PLATFORM_HAS_BUTTON
#include "dev/button-sensor.h"
//#endif

/** @addtogroup Debug Debug
 * @{
 */
#define DEBUG 1
#ifdef DEBUG
#include "net/ip/uip-debug.h"
#endif /* DEBUG */

#ifdef DEBUG_COOJA
static uint8_t prefix_set;  // COOJA Only
static uip_ipaddr_t prefix;// COOJA Only
#endif
/**
 * @}
 */

PROCESS(http_request_test_process, "Http Test Request");


static void get_callback(char *data, uint16_t len, int identifier, int state){
    PRINTF("GET Callback called \n");
    if(state < 0 ){
        PRINTF("GET ERROR: %s \n", data);
    }else{
        PRINTF("Received data: %s \n", data);
    }
}

static void post_callback(char *data, uint16_t len, int identifier, int state){
    PRINTF("POST Callback called \n");
    if(state < 0 ){
        PRINTF("POST ERROR: %s \n", data);
    }
}


PROCESS_THREAD(http_request_test_process, ev, data)
{
    static unsigned char i;

    PROCESS_BEGIN();

    /* Allow other processes to initialize properly. */
    for(i = 0; i < 10; ++i) {
        PROCESS_PAUSE();
    }

    PRINTF("HTTP Test Process Started \n");

    while(1) {
        PROCESS_WAIT_EVENT();
        if(ev == sensors_event && data == &button_sensor) {
            http_request_t get_req, post_req;
            get_req.callback = get_callback;
            get_req.identifier = 0;
            get_req.path = "/time/";
            process_post_synch(&http_request_process, http_request_get_event, &get_req);

            post_req.callback = post_callback;
            post_req.identifier = 0;
            post_req.path = "/time/";
            post_req.data = "data to send!";
            process_post_synch(&http_request_process, http_request_post_event, &post_req);
        }
    }
    PROCESS_END();
}
