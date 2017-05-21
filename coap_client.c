/*
 * coap_client.c
 *
 *  Created on: 20/05/2017
 *      Author: indrominadu
 */
#include "coap_client.h"
#include "er-coap-constants.h"
#include "er-coap-engine.h"

PROCESS(coap_client_process, "CoAP Client");

#define PROCESS_EVENT_CUSTOM             15

static coap_client_request_t request_list[MAX_REQUEST_COAP_CLIENT];
static int request_list_counter;


static void enqueue_request(httpd_state *data) {
    coap_client_request_t *p = &request_list[request_list_counter];

    // URI
    p->method = data->request_type;
    // URI
    memcpy(p->uri, data->action, data->action_len);
    p->uri[data->action_len] = '\0';
    // Node IP
    memcpy(p->ip, data->dst_ipaddr, sizeof(p->ip));

    if(data->request_type == REQUEST_TYPE_POST) {
        memcpy(p->buffer, data->buffer, data->content_length);
        p->blen = data->content_length;
    }

    request_list_counter++;
}

void
client_chunk_handler(void *response)
{
  const uint8_t *chunk;

  int len = coap_get_payload(response, &chunk);

  printf("|%.*s", len, (char *)chunk);
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();
    request_list_counter = 0;
    static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */

    PROCESS_PAUSE();

    while(1) {
        PROCESS_WAIT_UNTIL((ev == PROCESS_EVENT_CUSTOM || request_list_counter > 0));
        if(ev == PROCESS_EVENT_CUSTOM) {
            enqueue_request((httpd_state *)data);
            PROCESS_YIELD();
        }

        /* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
        coap_client_request_t *p = &request_list[request_list_counter - 1];
        coap_init_message(request, COAP_TYPE_CON, p->method, 0);

        coap_set_header_uri_path(request, p->uri);

        if(p->method == COAP_POST) {
            coap_set_payload(request, (uint8_t *)p->buffer, p->blen);
        }

        request_list_counter--;

        COAP_BLOCKING_REQUEST((uip_ip6addr_t *)p->ip, UIP_HTONS(COAP_DEFAULT_PORT), request,
                        client_chunk_handler);

    }

    PROCESS_END();
}
