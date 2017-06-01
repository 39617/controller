/*
 * coap_client.h
 *
 *  Created on: 20/05/2017
 *      Author: indrominadu
 */

#ifndef COAP_CLIENT_H_
#define COAP_CLIENT_H_

#include "httpd.h"
#include "er-http.h"

/* Number of max requests processing at same time by CoAP client */
#define COAP_CLIENT_MAX_REQUESTS                  1
#define COAP_CLIENT_FREE                          1
#define COAP_CLIENT_BUSY                          0
#define FREE                                      0x00
#define USED                                      0xFF

// CoAP client event
extern process_event_t coap_client_event_new_request;
//
extern int coap_client_current_req_number;
#define COAP_CLIENT_STATE                         (coap_client_current_req_number < COAP_CLIENT_MAX_REQUESTS)


typedef struct _coap_client_request {
    char uri[POST_PARAMS_VAL_MAX_LEN];
    char params[POST_PARAMS_VAL_MAX_LEN];
    uint8_t method;
    char buffer[MAX_PAYLOAD_SIZE];
    uint16_t blen;
    void * http_conn;
    unsigned int resp_status;
    unsigned int content_type;
    // usually point to a coap_node_entry_t
    void * owner;
} coap_client_request_t, *p_coap_client_request_t;

PROCESS_NAME(coap_client_process);

#endif /* COAP_CLIENT_H_ */
