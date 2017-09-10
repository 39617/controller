/**
* @file     res-coap_http.c
* @brief    Contains the resource for doing http requests after receiving it by coap
* @version  1.0
* @date     01 Aug. 2017
* @author   Tiago Costa & Ricardo Jesus & Claudio Prates
*
**/

/** @defgroup Resource Resource
 * @{
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "controller.h"
#include "res-common.h"
#include "http_request.h"
#include "node-table.h"
#include "netctrl-platform.h"

#define RES_COAPHTTP_PARAM_PATH  "p"

static char *path;
static char *payload;


/*---------------------------------------------------------------------------*/
static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);


static http_request_t resourse_request;

/*---------------------------------------------------------------------------*/

RESOURCE(res_coaptohttp,
          "CoAP to HTTP Node",
		  NULL,
		  res_post_handler,
		  NULL,
		  NULL); /*!< Creates a resource_t and marks it as CoAP */


static node_table_entry_t * get_identifier() {
	node_table_entry_t * node = node_table_get_node( (uip_ip6addr_t *)netctrl_get_nodeId());
	return (node == NULL)? NULL : node;
}



/**
 * @brief Common Handler on a CoAP request
 *
 * For methods not implemented
 *
 * @return nothing
 */
static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	PRINTF("POST res coap-http\n");
	node_table_entry_t * node = get_identifier();
	if(node == NULL) {
		REST.set_response_status(response, UNAUTHORIZED_4_01);
		return;
	} else {
		resourse_request.identifier = node->hash;
	}
#ifdef DEBUG
	int c;
	char *cursor;
#endif /* DEBUG */

	int len = REST.get_query_variable(request, RES_COAPHTTP_PARAM_PATH, (const char **)&path);
	resourse_request.path = path;
	resourse_request.path_len = len;
	if(resourse_request.path_len == 0 || resourse_request.path_len > PATH_MAX_SIZE) {
		REST.set_response_status(response, BAD_REQUEST_4_00);
		return;
	}
	PRINTF("Request with %d bytes and path: ", len);
	cursor = path;
	for(c = 0; c < len; c++, cursor++) {
		PRINTF("%c", *cursor);
	}
	PRINTF("\n");

	len = REST.get_request_payload(request, (const uint8_t **)&payload);
	resourse_request.payload = payload;
	resourse_request.payload_len = len;
	if(resourse_request.payload_len > PAYLOAD_MAX_SIZE) {
		REST.set_response_status(response, BAD_REQUEST_4_00);
		return;
	}
	PRINTF("Request with %d bytes and data: ", len);
	cursor = payload;
	for(c = 0; c < len; c++, cursor++) {
		PRINTF("%c", *cursor);
	}
	PRINTF("\n");

	resourse_request.callback = NULL;
	process_post_synch(&http_request_process, http_request_post_event, &resourse_request);
	REST.set_response_status(response, CONTENT_2_05);
}

/**
* @}
*/
