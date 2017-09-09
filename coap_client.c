/**
 * @file     coap_client.c
 * @brief    Contains the CoAP Proxy from HTTP (Transforms HTTP request to CoAP requests)
 * @version  1.0
 * @date     01 Jun. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#include "coap_client.h"
#include "node-table.h"
#include "httpd.h"
#include "er-http.h"
#include "error_codes.h"
#include "er-coap-constants.h"
#include "er-coap-engine.h"

/** @addtogroup Debug Debug
 * @{
 */
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
/**
 * @}
 */

PROCESS(coap_client_process, "CoAP Client"); /*!< Creates the Process coap_client_process with name CoAP Client */

static coap_client_request_t original_request[1];
int coap_client_current_req_number;

process_event_t coap_client_event_new_request; /*!< New Request Event */

static coap_packet_t request_packet[1];

/**
 * @brief   Reset the original request, set's 0 on the entire request
 * @return nothing
 */
static void reset_original_request(void)
{
    memset(original_request, 0x0, sizeof(coap_client_request_t));
}

/**
 * @brief Convert URI query
 *
 * Example query 'p1=param1;p2=param2' converts to 'p1=param1&p2=param2'
 *
 * @param query : query to convert
 * @param query_len : size of the entire query
 * @return nothing
 */
static void parse_uri_query(char *query, int query_len)
{
    int i;
    for (i = 0; i < query_len; i++)
    {
        if (query[i] == ISO_semi_colon)
        {
            *(&query[i]) = ISO_amp;
        }
    }
}

/**
 * @brief Converts HTTP Request to CoAP Request
 *
 * Takes the node_data and start proxing that data to CoAP
 *
 * @param dst_node : Destination node of http packet
 * @return nothing
 */
static void parse_request(node_table_entry_t *dst_node)
{
    reset_original_request();

    httpd_state * http_state = (httpd_state *) dst_node->node_data;
    // Set the corresponding Node object
    original_request->owner = (void *) dst_node;
    // store our connection
    original_request->http_conn = uip_conn;
    // Request Method
    original_request->method = http_state->request_type;

    // URI
    memcpy(original_request->uri, http_state->coap_req.action,
           MIN(http_state->coap_req.action_len, POST_PARAMS_VAL_MAX_LEN - 1));
    // guarantee the null terminator
    original_request->uri[MIN(http_state->coap_req.action_len,
                              POST_PARAMS_VAL_MAX_LEN - 1)] = '\0';

    // Parameters - are optional
    if (http_state->coap_req.params_len > 0)
    {
        memcpy(original_request->params,
               http_state->coap_req.params,
               MIN(http_state->coap_req.params_len,
                   POST_PARAMS_VAL_MAX_LEN - 1));
        // guarantee the null terminator
        original_request->params[MIN(http_state->coap_req.params_len,
                                     POST_PARAMS_VAL_MAX_LEN - 1)] = '\0';
        parse_uri_query(
                original_request->params,
                MIN(http_state->coap_req.params_len,
                    POST_PARAMS_VAL_MAX_LEN - 1));
    }

    // Request Type
    if (http_state->request_type == REQUEST_TYPE_POST)
    {
        memcpy(original_request->buffer, http_state->buffer,
               http_state->content_length);
        original_request->blen = http_state->content_length;
    }
}

/**
 * @brief Prepares the request
 *
 * Takes the destination node, proxy the http request to coap request
 *
 * @param dst_node : Pointer to the destination node
 * @return nothing
 */
static void prepare_request(node_table_entry_t *dst_node)
{

    parse_request(dst_node);

    /* prepare request, TID is set by COAP_BLOCKING_REQUEST() */
    coap_init_message(request_packet, COAP_TYPE_CON, original_request->method,
                      0);

    coap_set_header_uri_path(request_packet, original_request->uri);

    coap_set_header_uri_query(request_packet, original_request->params);

    if (original_request->method & METHOD_POST)
    {
        coap_set_payload(request_packet, (uint8_t *) original_request->buffer,
                         original_request->blen);
    }
}

/**
 * @brief Handles chunks transfer
 *
 * Takes the chunk received and copies to original_request buffer
 *
 * @param response : Pointer to the response packet
 * @return nothing
 */
void client_chunk_handler(void *response)
{
    coap_packet_t * res = (coap_packet_t *) response;
    const uint8_t *chunk;
    // select CoAP rest
    rest_select_if(COAP_IF);
    // Status code
    original_request->resp_status = res->code;
    original_request->content_type = res->content_format;

    int len = REST.get_request_payload(response, &chunk);
    memcpy(original_request->buffer, chunk, len);
    //res->user_data->blen = len;
    original_request->blen = len;
}

/**
 * @brief Main Thread of CoAP Client
 *
 * Responsible to control the request number
 *
 * Will receive event ev and the pointer to data regarding the event.
 *
 * @param coap_client_process : This process
 * @param ev : The event passed to this process
 * @param data : Data regarding to the event that fired
 * @return nothing
 */
PROCESS_THREAD(coap_client_process, ev, data)
{
    PROCESS_BEGIN();
        coap_client_current_req_number = 0;
        // owner of received requests
        static node_table_entry_t *owner;
        // Get an Event Id
        coap_client_event_new_request = process_alloc_event();

        PROCESS_PAUSE();

        while (1)
        {
            PROCESS_WAIT_UNTIL((ev == coap_client_event_new_request));
            PRINTF("\n**TODO COAP CLIENT!!!!!");

            //
            owner = (node_table_entry_t *) data;

            prepare_request((node_table_entry_t *) data);
            ((httpd_state *) owner->node_data)->response.immediate_response = 0;
            //
            coap_client_current_req_number++;

            // Make the CoAP request!
            COAP_BLOCKING_REQUEST(
                    (uip_ipaddr_t *)&((node_table_entry_t * )data)->ip_addr,
                    UIP_HTONS(COAP_DEFAULT_PORT), request_packet,
                    client_chunk_handler);
            // Send to httpd
            process_post_synch(&httpd_process, coap_client_event_new_response,
                               original_request);

            // free CoAP client
            coap_client_current_req_number--;

            // update the owner requests number
            owner->requests--;
        }

    PROCESS_END();
}
