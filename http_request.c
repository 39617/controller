

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


PROCESS(http_request_process, "Http Request");

process_event_t http_request_get_event;
process_event_t http_request_post_event;

//AUTOSTART_PROCESSES(&http_request_process);

//extern int contiki_argc;
//extern char **contiki_argv;

http_request_t local_req;
int action;

//http_request_list_t nodes [2];

char host[32] = "[fe80:2100::12:4bff:fe27:c50e]"; //!< Host of the server
uint16_t port = 3000; //!< Server port
int state;




/*LIST(freelist);
LIST(worklist);

http_request_list_t*/

/*#define MAX_URLLEN 128
static char url[MAX_URLLEN];

static uint8_t running;

static http_request *req;*/

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
        state = FREE;
        local_req.callback("Not a IP Address, cancelling request", NULL, local_req.identifier, GENERIC_ERROR);
        /*uip_ipaddr_t *addrptr;
        if(resolv_lookup(host, &addrptr) != RESOLV_STATUS_CACHED) {
          resolv_query(host);
          puts("Resolving host...");
          return;
        }
        uip_ipaddr_copy(&addr, addrptr);*/
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
    //unsigned char i;
    //static char host[32] = "[fe80:2100::12:4bff:fe27:c50e]";

    #if UIP_UDP
    //First check if the host is an IP address. - Currently we only accept IP addresses!
    if(uiplib_ipaddrconv(host, &addr) == 0) {
        //TODO only ip address for now
        /*PRINTF("Not a IP Address, cancelling request");
        local_req.callback(NULL, NULL, -1);*/
        state = FREE;
        local_req.callback("Not a IP Address, cancelling request", NULL, local_req.identifier, GENERIC_ERROR);
        /*uip_ipaddr_t *addrptr;
        if(resolv_lookup(host, &addrptr) != RESOLV_STATUS_CACHED) {
          resolv_query(host);
          puts("Resolving host...");
          return;
        }
        uip_ipaddr_copy(&addr, addrptr);*/
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
/*-----------------------------------------------------------------------------------*/
/*static void
app_quit(void)
{
  process_exit(&http_request_process);
  LOADER_UNLOAD();
}*/
/*-----------------------------------------------------------------------------------*/
PROCESS_THREAD(http_request_process, ev, data)
{
  //static char name[32];
  static unsigned char i;

  PROCESS_BEGIN();



  http_request_get_event = process_alloc_event();
  http_request_post_event = process_alloc_event();

  state = FREE;
 /* list_init(freelist);
  list_init(worklist);

  //add all nodes to freelist
  for(i = 0; i < LIST_SIZE; ++i) {
    list_add(freelist, &nodes[i]);
  }*/



  /* Allow other processes to initialize properly. */
  for(i = 0; i < 10; ++i) {
    PROCESS_PAUSE();
  }

  //start_get(); //dummy get

  puts("HTTP Request process started");

  while(1) {

    PROCESS_WAIT_EVENT();


//    #if PLATFORM_HAS_BUTTON
/*    if(ev == sensors_event && data == &button_sensor) {
        start_get();
    }*/
//    #endif /* PLATFORM_HAS_BUTTON */

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
        //state = RUNNING;
        //state = RUNNING;


        //cannot accept new requests
       /* if(list_length(freelist) == 0){
            ((p_http_request_t) data)->callback(NULL, NULL, -1);
        }
        p_http_request_list_t freenode = list_pop(freelist);
        freenode->item.callback = ((p_http_request_t) data)->callback;
        freenode->item.data = ((p_http_request_t) data)->data;
        freenode->item.path = ((p_http_request_t) data)->path;
        freenode->item.identifier = ((p_http_request_t) data)->identifier;

        list_add(worklist, freenode);*/

        //local_req.callback = ((p_http_request_t) data)->callback;
        /**************************************/
        //TODO we dont need to save this data right now, just copying the pointer, will need when implementing with lists!!!!
        //strcpy(local_req.data, ((p_http_request_t) data)->data);
        //strcpy(local_req.path, ((p_http_request_t) data)->path);
        //local_req.data = ((p_http_request_t) data)->data;
        //local_req.path = ((p_http_request_t) data)->path;
        /**************************************/
        //local_req.identifier = ((p_http_request_t) data)->identifier;
        //start_get();

    }else if (ev == http_request_post_event){
        if(state == RUNNING){
            //cannot accept new request
            ((p_http_request_t) data)->callback("Not free", NULL, local_req.identifier, GENERIC_ERROR);
            //((p_http_request_t) data)->callback(NULL, NULL, -1);

        }else{
            state = RUNNING;
            local_req.callback = ((p_http_request_t) data)->callback;
            local_req.data = ((p_http_request_t) data)->data;
            local_req.path = ((p_http_request_t) data)->path;
            local_req.identifier = ((p_http_request_t) data)->identifier;
            start_post();
        }

        //state = RUNNING;
        //cannot accept new requests
        /*if(list_length(freelist) == 0){
            ((p_http_request_t) data)->callback(NULL, NULL, -1);
        }
        p_http_request_list_t freenode = list_pop(freelist);
        freenode->item.callback = ((p_http_request_t) data)->callback;
        freenode->item.data = ((p_http_request_t) data)->data;
        freenode->item.path = ((p_http_request_t) data)->path;
        freenode->item.identifier = ((p_http_request_t) data)->identifier;

        list_add(worklist, freenode);*/
        //TODO
        //PRINTF("POST still need to be done \n");
        //local_req.callback = ((p_http_request_t) data)->callback;
        /**************************************/
        //TODO we dont need to save this data right now, just copying the pointer, will need when implementing with lists!!!!
        //strcpy(local_req.data, ((p_http_request_t) data)->data);
        //strcpy(local_req.path, ((p_http_request_t) data)->path);
        //local_req.data = ((p_http_request_t) data)->data;
        //local_req.path = ((p_http_request_t) data)->path;
        /**************************************/
        //local_req.identifier = ((p_http_request_t) data)->identifier;
        //PRINTF("starting post \n");
        //start_post();

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

  //app_quit();
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
    state = FREE;
  puts("Done.");
  //app_quit();
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




  //int ret;

  /*if(len > 0) {
    dload_bytes += len;
    printf("Downloading (%lu bytes)\n", dload_bytes);
    if(file != -1) {
      ret = cfs_write(file, data, len);
      if(ret != len) {
        printf("Wrote only %d bytes\n", ret);
      }
    }
  }*/

  /*if(data == NULL) {
    //TODO REMOVER LISTA
      puts("remove lista pah!");
  }*/
  //app_quit();
}
/*-----------------------------------------------------------------------------------*/
