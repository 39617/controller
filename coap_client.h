/*
 * coap_client.h
 *
 *  Created on: 20/05/2017
 *      Author: indrominadu
 */

#ifndef COAP_CLIENT_H_
#define COAP_CLIENT_H_

#include "er-http.h"

#define MAX_REQUEST_COAP_CLIENT          1
#define PROCESS_EVENT_CUSTOM             15

typedef struct _coap_client_request {
    char uri[MAX_URI_LEN];
    char ip[16];
    uint8_t method;
    char buffer[MAX_PAYLOAD_SIZE];
    uint16_t blen;
    void * connection;
    unsigned int status_code;
} coap_client_request_t, *p_coap_client_request_t;

PROCESS_NAME(coap_client_process);

#endif /* COAP_CLIENT_H_ */
