/**
 * @file     controller.h
 * @brief    Contains the Controller definitions
 * @version  1.0
 * @date     01 Aug. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#include "http_request.h"
#include "contiki.h"

#include "webclient.h"

#include "process.h"

#include <stdio.h>
#include <string.h>

//#include "lib/list.h"

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

/** @defgroup HTTP_Request HTTP_Request
 * @{
 */


PROCESS(http_request_process, "Http Request");

process_event_t http_request_get_event;
process_event_t http_request_post_event;


http_request_t local_req;
int action;

//http_request_list_t nodes [2];

char host[32] = "[fe80:2100::12:4bff:fe27:c50e]"; //!< Host of the server
uint16_t port = 3000; //!< Server port
int state;



/*-----------------------------------------------------------------------------------*/
/* Called when the URL present in the global "url" variable should be
 * opened. It will call the hostname resolver as well as the HTTP
 * client requester.
 */
static void start_get(void){
    uip_ipaddr_t addr;

    #if UIP_UDP
    //First check if the host is an IP address. - Currently we only accept IP addresses!
    if(uiplib_ipaddrconv(host, &addr) == 0) {
        //TODO only ip address for now
        state = FREE;
        local_req.callback("Not a IP Address, cancelling request", NULL, local_req.identifier, GENERIC_ERROR);

    }
    #else /* UIP_UDP */
    //By now we have a valid IP address
    uiplib_ipaddrconv(host, &addr);
    #endif /* UIP_UDP */


    if(webclient_get(host,port, local_req.path) == 0) {
        state = FREE;
        local_req.callback("Out of memory error", NULL, local_req.identifier, GENERIC_ERROR);
    } else {
        //state = RUNNING;
        action = GET;
        PRINTF("Connecting... \n");
    }


}
/*-----------------------------------------------------------------------------------*/
static void start_post(void){
    uip_ipaddr_t addr;

    #if UIP_UDP
    //First check if the host is an IP address. - Currently we only accept IP addresses!
    if(uiplib_ipaddrconv(host, &addr) == 0) {
        //TODO only ip address for now
        state = FREE;
        local_req.callback("Not a IP Address, cancelling request", NULL, local_req.identifier, GENERIC_ERROR);

    }
    #else /* UIP_UDP */
    //By now we have a valid IP address
    uiplib_ipaddrconv(host, &addr);
    #endif /* UIP_UDP */


    if(webclient_post(host,port, local_req.path, local_req.data) == 0) {
        state = FREE;
        local_req.callback("Out of memory error", NULL, local_req.identifier, GENERIC_ERROR);
        //PRINTF("Out of memory error");
    } else {
        //state = RUNNING;
        action = POST;
        PRINTF("Connecting... \n");
    }


}

PROCESS_THREAD(http_request_process, ev, data)
{
  static unsigned char i;

  PROCESS_BEGIN();



  http_request_get_event = process_alloc_event();
  http_request_post_event = process_alloc_event();

  state = FREE;


  /* Allow other processes to initialize properly. */
  for(i = 0; i < 10; ++i) {
    PROCESS_PAUSE();
  }

  puts("HTTP Request process started");

  while(1) {

    PROCESS_WAIT_EVENT();

    if (ev == http_request_get_event){
        if(state == RUNNING){
            //cannot accept new request
            ((p_http_request_t) data)->callback("Not free", NULL, local_req.identifier, GENERIC_ERROR);
        }else{
            state = RUNNING;
            local_req.callback = ((p_http_request_t) data)->callback;
            local_req.data = ((p_http_request_t) data)->data;
            local_req.path = ((p_http_request_t) data)->path;
            local_req.identifier = ((p_http_request_t) data)->identifier;
            start_get();
        }

    }else if (ev == http_request_post_event){
        if(state == RUNNING){
            //cannot accept new request
            ((p_http_request_t) data)->callback("Not free", NULL, local_req.identifier, GENERIC_ERROR);

        }else{
            state = RUNNING;
            local_req.callback = ((p_http_request_t) data)->callback;
            local_req.data = ((p_http_request_t) data)->data;
            local_req.path = ((p_http_request_t) data)->path;
            local_req.identifier = ((p_http_request_t) data)->identifier;
            start_post();
        }

    }else if(ev == tcpip_event) {
      webclient_appcall(data);
    #if UIP_UDP
    } else if(ev == resolv_event_found) {
      /* Either found a hostname, or not. */
      if((char *)data != NULL &&
        resolv_lookup((char *)data, NULL) == RESOLV_STATUS_CACHED) {
        start_get();
      } else {
        puts("Host not found");
        //app_quit();
      }
    #endif /* UIP_UDP */
    }
  }

  PROCESS_END();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection was abruptly aborted.
 */
void
webclient_aborted(void)
{
    local_req.callback("Connection reset by peer", NULL, local_req.identifier, GENERIC_ERROR);
    state = FREE;
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection timed out.
 */
void
webclient_timedout(void)
{
  //puts("Connection timed out");
  local_req.callback("Connection timed out", NULL, local_req.identifier, GENERIC_ERROR);
  state = FREE;
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection was closed after a request from the "webclient_close()"
 * function. .
 */
void
webclient_closed(void)
{
    state = FREE;
  puts("Done.");
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection is connected.
 */
void
webclient_connected(void)
{
  puts("Request sent...");
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient module when HTTP data
 * has arrived.
 */
void
webclient_datahandler(char *data, uint16_t len)
{


  //static unsigned long dload_bytes;

    if(len > 0 || action == POST) {
        local_req.callback(data, len, local_req.identifier, SUCCESS);
    }

    if(data == NULL )
        state = FREE;
}


/**
 * @}
 */
