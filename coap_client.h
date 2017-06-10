/**
 * @file     coap_client.h
 * @brief    Contains the Definitions of the Coap Client
 * @version  1.0
 * @date     20 May. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#ifndef COAP_CLIENT_H_
#define COAP_CLIENT_H_

/** @defgroup CoAP CoAP
 * @{
 */

/** @addtogroup CoAP_Client
 * @{
 */

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

typedef struct _coap_client_request
{
    char uri[POST_PARAMS_VAL_MAX_LEN]; /*!< The URI of CoAP request */
    char params[POST_PARAMS_VAL_MAX_LEN]; /*!< The Params of CoAP request */
    uint8_t method; /*!< The method of CoAP request GET | PUT | DELETE | POST */
    char buffer[MAX_PAYLOAD_SIZE]; /*!< The payload of The CoAP request */
    uint16_t blen; /*!< The size of the Payload */
    void * http_conn; /*!< The HTTP tcp connection that we are proxing to CoAP */
    unsigned int resp_status; /*!< The Response Status code */
    unsigned int content_type; /*!< The content-type of the request mapped from http */
    void * owner; /*!< Pointer to the owner node */
} coap_client_request_t, *p_coap_client_request_t;

PROCESS_NAME(coap_client_process);
/**
 * @}
 */

/**
 * @}
 */

#endif /* COAP_CLIENT_H_ */
