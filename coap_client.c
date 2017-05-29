/*
 * coap_client.c
 *
 *  Created on: 20/05/2017
 *      Author: indrominadu
 */
#include "coap_client.h"
#include "coap_node.h"
#include "httpd.h"
#include "er-http.h"
#include "error_codes.h"
#include "er-coap-constants.h"
#include "er-coap-engine.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif


PROCESS(coap_client_process, "CoAP Client");

static coap_client_request_t request_list[COAP_CLIENT_MAX_REQUESTS];
int coap_client_current_req_number;
//
process_event_t coap_client_event_new_request;
//
static coap_packet_t request[1];



static coap_client_request_t * alloc_request_slot(void) {
	int i;

	for(i = 0; i < COAP_CLIENT_MAX_REQUESTS; i++) {
		if(request_list[i].state == FREE) {
			coap_client_current_req_number++;
			request_list[i].state = USED;
			return &request_list[i];
		}
	}

	return NULL;
}

static void free_request_slot(coap_client_request_t * slot) {
	int i;

	for(i = 0; i < COAP_CLIENT_MAX_REQUESTS; i++) {
		if(&request_list[i] == slot) {
			memset(&request_list[i], 0x0, sizeof(coap_client_request_t));
			return;
		}
	}
}

static coap_client_request_t * parse_request(coap_node_entry_t *dst_node) {
	coap_client_request_t *p = alloc_request_slot();

	httpd_state * http_state = (httpd_state *)dst_node->node_data;
	// Set the corresponding Node object
	p->owner = (void *)dst_node;
	// store our connection
	p->http_conn = uip_conn;
	// Request Method
	p->method = http_state->request_type;
	// URI
	memcpy(p->uri, http_state->action, MIN(http_state->action_len, MAX_URI_LEN - 1));
	// guarantee the null terminator
	p->uri[MIN(http_state->action_len, MAX_URI_LEN)] = '\0';

	if(http_state->request_type == REQUEST_TYPE_POST) {
		memcpy(p->buffer, http_state->buffer, http_state->content_length);
		p->blen = http_state->content_length;
	}

	return p;
}

static coap_client_request_t * get_corresponding_req(int received_mid) {
	int i;

	for(i = 0; i < COAP_CLIENT_MAX_REQUESTS; i++) {
		if(request_list[i].mid == received_mid) {
			return &request_list[i];
		}
	}

	return NULL;
}

static void prepare_request(coap_node_entry_t *dst_node) {
	coap_client_request_t * coap_req = parse_request(dst_node);

	/* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
	coap_init_message(request, COAP_TYPE_CON, coap_req->method, 0);

	coap_set_header_uri_path(request, coap_req->uri);

	if(coap_req->method == COAP_POST) {
		coap_set_payload(request, (uint8_t *)coap_req->buffer, coap_req->blen);
	}

	request->user_data = coap_req;
}

void
client_chunk_handler(void *response)
{
    coap_packet_t * res = (coap_packet_t *)response;
    const uint8_t *chunk;

    coap_client_request_t * req = get_corresponding_req(res->mid);
    if(req == NULL) {
    	fazer alguma coisa... Ou talvez não (apenas retorna) uma vez que cada request deve ter um timer que ao expirar liberta o slot e talvez envie resposta de erro para o httpd?
    }
    //
    req->resp_status = res->code;

    if(coap_get_rest_method(res) == COAP_GET){
        int len = coap_get_payload(response, &chunk);
        memcpy(req->buffer, res->payload, res->payload_len);
        //res->user_data->blen = len;
        req->blen = len;
    }

    //printf("|%.*s", res->user_data->blen, (char *)chunk);

    // TODO: deve estar depois do COAP_BLOCKING_REQUEST, uma vez que só aí temos a garantia de que o pedido terminou...
    // O req passa para fora para poder ser acedido lá?
    process_post_synch(&httpd_process, coap_client_event_new_response, req);

    free_request_slot(req);
}

PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();
    coap_client_current_req_number = 0;
    // owner of received requests
    static coap_node_entry_t *owner;
    // Get an Event Id
    coap_client_event_new_request = process_alloc_event();

    PROCESS_PAUSE();

    while(1) {
        PROCESS_WAIT_UNTIL((ev == coap_client_event_new_request));
        PRINTF("\n**TODO COAP CLIENT!!!!!");

        //
        owner = (coap_node_entry_t *)data;

		if(coap_client_current_req_number < COAP_CLIENT_MAX_REQUESTS) {
			prepare_request((coap_node_entry_t *)data);
			((httpd_state *)owner->node_data)->response.immediate_response = 0;

			// Do the request!
			COAP_BLOCKING_REQUEST((uip_ip6addr_t *)((coap_node_entry_t *)data)->ip,
					UIP_HTONS(COAP_DEFAULT_PORT), request, client_chunk_handler);
		}
		else {
			((httpd_state *)owner->node_data)->response.immediate_response = 1;
			PRINTF("\n** CoAP Cli. Too many req!");
			set_http_error(owner->node_data, error_too_many_requests, REST.status.SERVICE_UNAVAILABLE);
        }

        //PROCESS_YIELD();
		// TODO: tirar o martelo!
        ev = 0;
    }

    PROCESS_END();
}
