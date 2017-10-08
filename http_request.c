#include "http_request.h"
#include "contiki.h"

#include "webclient.h"

#include "process.h"

#include <stdio.h>
#include <string.h>


#define DEBUG 0
#ifdef DEBUG
#include "net/ip/uip-debug.h"
#endif /* DEBUG */


PROCESS(http_request_process, "Http Request");

process_event_t http_request_get_event;
process_event_t http_request_post_event;
//
http_request_t *local_req;
int action;

char host[] = "fe80:2100::12:4bff:fe27:c50e"; //!< Host of the server
uint16_t port = 3000; //!< Server port
int state;

/*-----------------------------------------------------------------------------------*/
/* Called when the URL present in the global "url" variable should be
 * opened. It will call the hostname resolver as well as the HTTP
 * client requester.
 */
static void start_get(void){
    uip_ipaddr_t addr;
    //unsigned char i;
    //static char host[32] = "[fe80:2100::12:4bff:fe27:c50e]";

    #if UIP_UDP
    //First check if the host is an IP address. - Currently we only accept IP addresses!
    if(uiplib_ipaddrconv(host, &addr) == 0) {
        //TODO only ip address for now
        //PRINTF("Not a IP Address, cancelling request");
        state = IDLE;
        if(local_req->callback != NULL) {
        	local_req->callback("Not a IP Address, cancelling request", 0, local_req->identifier, GENERIC_ERROR);
        }
    }
    #else /* UIP_UDP */
    //By now we have a valid IP address
    uiplib_ipaddrconv(host, &addr);
    #endif /* UIP_UDP */


    if(webclient_get(host,port, local_req->path) == 0) {
        state = IDLE;
        if(local_req->callback != NULL) {
        	local_req->callback("Out of memory error", 0, local_req->identifier, GENERIC_ERROR);
        }
    } else {
        //state = RUNNING;
        action = GET;
        PRINTF("Connecting... \n");
    }


}
/*-----------------------------------------------------------------------------------*/
static void start_post(uint32_t requester_hash){
    uip_ipaddr_t addr;
    #if UIP_UDP
    //First check if the host is an IP address. - Currently we only accept IP addresses!
    if(uiplib_ipaddrconv(host, &addr) == 0) {
        //TODO only ip address for now
        /*PRINTF("Not a IP Address, cancelling request");
        local_req->callback(NULL, NULL, -1);*/
        state = IDLE;
        if(local_req->callback != NULL) {
        	local_req->callback("Not a IP Address, cancelling request", 0, local_req->identifier, GENERIC_ERROR);
        }
    }
    #else /* UIP_UDP */
    //By now we have a valid IP address
    uiplib_ipaddrconv(host, &addr);
    #endif /* UIP_UDP */


    if(webclient_post(host,port, local_req->path, local_req->path_len, local_req->payload,
    		local_req->payload_len, requester_hash) == 0) {
        state = IDLE;
        if(local_req->callback != NULL) {
        	local_req->callback("Out of memory error", 0, local_req->identifier, GENERIC_ERROR);
        }
        //PRINTF("Out of memory error");
    } else {
        //state = RUNNING;
        action = POST;
        PRINTF("Connecting... \n");
    }


}
/*-----------------------------------------------------------------------------------*/
PROCESS_THREAD(http_request_process, ev, data)
{
  //static char name[32];
  static unsigned char i;

  PROCESS_BEGIN();

  http_request_get_event = process_alloc_event();
  http_request_post_event = process_alloc_event();

  state = IDLE;

  /* Allow other processes to initialize properly. */
  for(i = 0; i < 10; ++i) {
    PROCESS_PAUSE();
  }

  PRINTF("HTTP Request process started\n");

  while(1) {

    PROCESS_WAIT_EVENT();

    if (ev == http_request_get_event){
    	local_req = data;
        if(state == RUNNING){
        	if(local_req->callback != NULL) {
        		//cannot accept new request
        		local_req->callback("Not free", 0, local_req->identifier, GENERIC_ERROR);
        	}
        }else{
            state = RUNNING;
            local_req = (p_http_request_t) data;
            start_get();
        }

    }else if (ev == http_request_post_event){
    	local_req = data;
        if(state == RUNNING){
            //cannot accept new request
        	if(local_req->callback != NULL) {
        		local_req->callback("Not free", 0, local_req->identifier, GENERIC_ERROR);
        	}

        }else{
            state = RUNNING;
            local_req = (p_http_request_t) data;
            start_post(local_req->node_hash);
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
	if(local_req->callback != NULL) {
		local_req->callback("Connection reset by peer", 0, local_req->identifier, GENERIC_ERROR);
	}
    state = IDLE;
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection timed out.
 */
void
webclient_timedout(void)
{
  //puts("Connection timed out");
  if(local_req->callback != NULL) {
    local_req->callback("Connection timed out", 0, local_req->identifier, GENERIC_ERROR);
  }
  PRINTF("Webclient Connection timed out...\n");
  state = IDLE;
  //app_quit();
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection was closed after a request from the "webclient_close()"
 * function. .
 */
void
webclient_closed(void)
{
  state = IDLE;
  PRINTF("Connection closed.\n");
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient when the HTTP
 * connection is connected.
 */
void
webclient_connected(void)
{
	PRINTF("Webclient Connected...\n");
}
/*-----------------------------------------------------------------------------------*/
/* Callback function. Called from the webclient module when HTTP data
 * has arrived.
 */
void
webclient_datahandler(char *data, uint16_t len)
{
    if(len > 0 || action == POST) {
    	if(local_req->callback != NULL) {
    		local_req->callback(data, len, local_req->identifier, SUCCESS);
    	}
    }

    if(data == NULL ) {
        state = IDLE;
    }
}
/*-----------------------------------------------------------------------------------*/
