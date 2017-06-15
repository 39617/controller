/**
* @file     res-coap_nodes.c
* @brief    Contains a Resource of a specific endpoint (Can be CoAP resource or HTTP resource)
* @version  1.0
* @date     01 Jun. 2017
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
#include "coap_node.h"

/* NOTE: Currently supports only *ONE* request per node. */
#define MAX_REQUESTS_PER_NODE                    1  /*!< Max requests per node */

#define POST_PARAMS_N							 1  /*!< Max parameter expected on POST requests */
//
#define TARGET_PARAM                             "t" /*!< Query Param for Target */
#define ACTION_PARAM                             "a" /*!< Query Param for Action */
#define PARAMS_PARAM                             "p" /*!< Query Param for Params */
/*---------------------------------------------------------------------------*/
static void
res_common_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
/*---------------------------------------------------------------------------*/
RESOURCE_HTTP(res_coapnodes,
          "CoAP Node",
		  res_common_handler,
		  NULL,
		  res_common_handler,
          NULL); /*!< Creates a resource_t and marks it as HTTP */
/*---------------------------------------------------------------------------*/
static const char *target_str; /*!< Its the t={x} in the Query | the target as string |  */
static uint32_t target_hash; /*!< Target as Hash as uint32_t */
static const char *action; /*!< Its the a={x} in the Query | the action as string |  */
static const char *params; /*!< Its the p={x} in the Query | the params as string |  */
// lengths
int target_len; /*!< Size of target */
int action_len; /*!< Size of action */
int params_len; /*!< Size of params */
/*---------------------------------------------------------------------------*/
parse_pair_t post_key_pair[POST_PARAMS_N]; /*!< POST parameters are stored here */

/**
 * @brief Parses the target query param
 * @return size_t
 */
static size_t parse_target_str() {
	// TODO: Melhorar este 'algoritmo'. Com esta função acontece existem falhas como:
	// 1234A é um numero válido e a função vai retornar 1234.
	// Em todo o caso não é critico.
	target_hash = atoi(target_str);

	return (target_hash > 0)? 1 : 0;
}

/**
 * @brief Common Handler on a HTTP request
 *
 * Takes a HTTP request and "files" a CoAP request
 *
 * @return nothing
 */
static void
res_common_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	http_response * rsp = (http_response *)response;
	httpd_state * req = (httpd_state *)request;

	/* -----------------  parse query parameters  ----------------- */
	target_len = REST.get_query_variable(request, TARGET_PARAM, &target_str);
	PRINTF("** Target with len %d: ", target_len);
	int i; /* for PRINT_ARRAY */
	const char * ptr = target_str;
	PRINT_ARRAY(ptr, target_len);
	if(target_len <= 0 || !parse_target_str()) {
		set_http_error(request, error_invalid_params, REST.status.BAD_REQUEST);
		return;
	}

	action_len = REST.get_query_variable(request, ACTION_PARAM, &action);
	PRINTF("\n** Action with len %d: ", action_len);
	ptr = action;
	PRINT_ARRAY(ptr, action_len);
	if(action_len <= 0) {
		set_http_error(request, error_invalid_params, REST.status.BAD_REQUEST);
		return;
	}

	/* Parameters are optional but need to be valid if present! */
	params_len = REST.get_query_variable(request, PARAMS_PARAM, &params);
	PRINTF("\n** Params with len %d: ", params_len);
	ptr = params;
	PRINT_ARRAY(ptr, params_len);
	if(params_len >= POST_PARAMS_VAL_MAX_LEN) {
		set_http_error(request, error_invalid_params, REST.status.BAD_REQUEST);
		return;
	}

	/* -----------------  get node status  ----------------- */
	coap_node_entry_t * node;
	if(!get_coap_node(target_hash, &node)) {
		PRINTF("\n** Target node Not Found!");
		set_http_error(request, error_node_not_found, REST.status.NOT_FOUND);
		return;
	}
	/* ----------------------------------------------------- */

	/* ---------  check if the requested node is free to get a request  --------- */
	if(node->requests >= MAX_REQUESTS_PER_NODE) {
		PRINTF("\n** Node Unavailable!");
		set_http_error(request, error_node_unavailable, REST.status.SERVICE_UNAVAILABLE);
		return;
	}
	/* ----------------------------------------------------- */

	/* -----------------  begin request build  ----------------- */
	// update node requests
	node->requests += 1;
	// fill the request
	req->coap_req.action = (char*)action;
	req->coap_req.action_len = action_len;
	// parameters are optional
	if(params_len > 0) {
		req->coap_req.params = (char*)params;
		req->coap_req.params_len = params_len;
	}
	// associate the base request
	node->node_data = (void *)request;

	/* CoAP client only process one request at a time, so we need to check if is free. */
	// TODO: implement a queue to dispatch requests to CoAP client
	if(COAP_CLIENT_STATE == COAP_CLIENT_FREE) {
		// httpd will not respond when return
		// NOTE: Need to be here before 'process_post_synch' because
		//       CoAP client can change it.
		//       Ex.: when it can't accept more requests. In case, the
		//       response will be immediate.
		rsp->immediate_response = 0;
		//
		process_post_synch(&coap_client_process,
				coap_client_event_new_request, (process_data_t)node);

		// make httpd ignore periodic/retry events for this connection
		req->state = STATE_IGNORE;
	}
	else {
		rsp->immediate_response = 1;
		PRINTF("\n** CoAP Cli. Too many req!");
		set_http_error(request, error_too_many_requests, REST.status.SERVICE_UNAVAILABLE);
	}
}
/*---------------------------------------------------------------------------*/


//static int
//get_post_param(char *param, size_t len, char *ptr) {
//	int i;
//	for(i = 0; i < POST_PARAMS_N; i++) {
//		if(memcmp(param, &post_key_pair[i], len)) {
//			*ptr = &post_key_pair[i];
//			return 1;
//		}
//	}
//
//	return 0;
//}
//
///**
// * @brief POST Handler on a HTTP request
// *
// * Takes a HTTP request and fill in a CoAP request
// *
// * @return nothing
// */
//static void
//res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
//{
//	httpd_state *packet = (httpd_state *)request;
//
//#if DEBUG
//	PRINTF("* POST PAYLOAD\n");
//	char * ptr = (char *)&packet->buffer;
//	int i;
//	PRINT_ARRAY(ptr, packet->content_length);
//	PRINTF("\n");
//#endif
//
//	size_t ret = parse_post_param(packet->buffer, packet->content_length, (p_parse_pair_t)&post_key_pair, POST_PARAMS_N);
//	if(ret == PARSE_POST_STATE_ERROR) {
//		set_http_error(request, error_invalid_params, REST.status.BAD_REQUEST);
//		// TODO: fazer o quê?
//		http_set_redir_path(request, "/index2"); /* propositado para dar not found */
//		return;
//	}
//
//	/* -----------------  parse query parameters  ----------------- */
//	ret = get_post_param("t=", 2, &target_str);
//	PRINTF("** Target with len %d: ", target_len);
//	int i; /* for PRINT_ARRAY */
//	const char * ptr = target_str;
//	PRINT_ARRAY(ptr, target_len);
//	if(target_len <= 0 || !parse_target_str()) {
//		set_http_error(request, error_invalid_params, REST.status.BAD_REQUEST);
//		return;
//	}
//
//	action_len = REST.get_query_variable(request, ACTION_PARAM, &action);
//	PRINTF("\n** Action with len %d: ", action_len);
//	ptr = action;
//	PRINT_ARRAY(ptr, action_len);
//	if(action_len <= 0) {
//		set_http_error(request, error_invalid_params, REST.status.BAD_REQUEST);
//		return;
//	}
//
//	/* Parameters are optional but need to be valid if present! */
//	params_len = REST.get_query_variable(request, PARAMS_PARAM, &params);
//	PRINTF("\n** Params with len %d: ", params_len);
//	ptr = params;
//	PRINT_ARRAY(ptr, params_len);
//	if(params_len >= POST_PARAMS_VAL_MAX_LEN) {
//		set_http_error(request, error_invalid_params, REST.status.BAD_REQUEST);
//		return;
//	}
//
//
//
//
//#if DEBUG
//	PRINTF("Key.0: %s", post_key_pair[0].key);
//	PRINTF(" * Value.0: %s\n", post_key_pair[0].value);
//	PRINTF("Key.1: %s", post_key_pair[1].key);
//	PRINTF(" * Value.1: %s\n", post_key_pair[1].value);
//#endif
//
//
//}
/**
 * @}
 */
 
/**
* @}
*/
