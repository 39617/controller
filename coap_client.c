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

static coap_client_request_t request_list[MAX_REQUEST_COAP_CLIENT];
int coap_client_current_req_number;


static void enqueue_request(httpd_state *data) {
    coap_client_request_t *p = &request_list[coap_client_current_req_number];
    p->connection = data->currentConnection;
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

    coap_client_current_req_number++;
}

void
client_chunk_handler(void *response)
{
    coap_packet_t * res = (coap_packet_t *)response;
    const uint8_t *chunk;

    //TODO: mapping
    request_list[0].status_code = res->code;

    if(coap_get_rest_method(res) == COAP_GET){

        int len = coap_get_payload(response, &chunk);
        memcpy(&request_list[0].buffer, res->payload, res->payload_len);
        //res->user_data->blen = len;
        request_list[0].blen = len;
    }

    //printf("|%.*s", res->user_data->blen, (char *)chunk);

    process_post_synch(&httpd_process, PROCESS_EVENT_CUSTOM, &request_list[0]);

    // TODO
    coap_client_current_req_number--;
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();
    coap_client_current_req_number = 0;
    static coap_packet_t request[1];      /* This way the packet can be treated as pointer as usual. */

    PROCESS_PAUSE();

    while(1) {
        PROCESS_WAIT_UNTIL((ev == PROCESS_EVENT_CUSTOM || coap_client_current_req_number > 0));
        if(ev == PROCESS_EVENT_CUSTOM) {
            if(coap_client_current_req_number < MAX_REQUEST_COAP_CLIENT) {
                enqueue_request((httpd_state *)data);
            }
            //PROCESS_YIELD();
        } else if(coap_client_current_req_number > 0){

            /* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
            coap_client_request_t *p = &request_list[coap_client_current_req_number - 1];
            coap_init_message(request, COAP_TYPE_CON, p->method, 0);

            coap_set_header_uri_path(request, p->uri);

            if(p->method == COAP_POST) {
                coap_set_payload(request, (uint8_t *)p->buffer, p->blen);
            }

            request->user_data = &request_list[coap_client_current_req_number - 1];
            //coap_client_current_req_number--;

            COAP_BLOCKING_REQUEST((uip_ip6addr_t *)p->ip, UIP_HTONS(COAP_DEFAULT_PORT), request,
                            client_chunk_handler);
        }

        //PROCESS_YIELD();
        ev = 0;
    }

    PROCESS_END();
}
