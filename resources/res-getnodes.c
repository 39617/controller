/**
* @file     res-getnodes.c
* @brief    Contains a Resource of a specific endpoint (Can be CoAP resource or HTTP resource)
* @version  1.0
* @date     01 Setembro. 2017
* @author   Tiago Costa & Ricardo Jesus & Claudio Prates
*
**/

/** @defgroup Resource Resource
 * @{
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "controller.h"
#include "res-common.h"
#include "node-table.h"
#include "er-http.h"
//
#define TYPE_PARAM                             "t" /*!< Query Param for Type */
#define TYPE_PARAM_MAX_LEN		     			3 /*!< Because it's a char, its max values is 255 */
/*---------------------------------------------------------------------------*/
static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
/*---------------------------------------------------------------------------*/
RESOURCE_HTTP(res_getnodes,
          "GetNodes",
		  res_get_handler,
		  NULL,
		  NULL,
          NULL); /*!< Creates a resource_t and marks it as HTTP */
/*---------------------------------------------------------------------------*/
static const char *type_str; /*!< Its the t={x} in the Query | the target as string |  */
static uint8_t type_filter; /*!< Filter Node's Table by this type if present */

// lengths
int type_len; /*!< Size of target */
/*---------------------------------------------------------------------------*/

// TODO: maybe the size of the keys should be reduced. Ex.: eq_type => et
static char obj_begin[] = "{\"list\":[";
static char obj_hash[] = "{\"hash\":%lu,";
static char obj_hash_with_prev[] = ",{\"hash\":%lu,";
static char *obj_hash_ptr;
static char obj_eq_type[] = "\"eq_type\":%d,";
static char obj_data[] = "\"data\":%d,";
static char obj_uptime[] = "\"uptime\":%d}";
static char obj_end[] = "]}";
static int build_nodes_list(void * buffer, size_t buf_len) {
	/* !!! Data written to the arg 'buffer' MUST end with Null terminator! Otherwise the behavior is unknown */
	/*
	 {
	   [
	 	 {
	 	 	 “hash”:1234567890,
	 	 	 “eq type”:1,
	 	 	 “data”:2736452637,
	 	 	 “uptime”:7463625
	 	 },
	 	 {
	 	 	 “hash”:1234567890,
	 	 	 “eq type”:1,
	 	 	 “data”:2736452637,
	 	 	 “uptime”:7463625
	 	 }
	   ]
	 }
	 */
	static node_table_iterator_t it;
	static int step;
	int len;

	// Need to initialize the iterator in the first call
	if(step == 0) {
		node_table_init_iterator(&it);
		obj_hash_ptr = obj_hash;
	}

	switch(step) {
	case 0:
		memcpy(buffer, obj_begin, sizeof(obj_begin));
		len = sizeof(obj_begin);
		step++;
		break;
	case 1:
get_next_node:
		// Empty table?
		if(node_table_iterate(&it) == 0) {
			// Apply filter if exists
			if((type_len > 0) && (it.node->type != type_filter)) {
				// Get another one
				goto get_next_node;
			}
/* ----------------------------- Body -------------------------------------- */
			len = snprintf((char *)buffer, buf_len, obj_hash_ptr, it.node->hash);
			step++;
		} else {
			// Empty Table or last node. Close the JSON object
			goto case_5;
		}
		break;
	case 2:
		len = snprintf((char *)buffer, buf_len, obj_eq_type, it.node->type);
		step++;
		break;
	case 3:
		len = snprintf((char *)buffer, buf_len, obj_data, it.node->data);
		step++;
		break;
	case 4:
		// TODO: this is a naive aproach. For a correct aproach we need an RTC working with NTP.
		len = snprintf((char *)buffer, buf_len, obj_uptime,  (clock_time() - it.node->timestamp) /  CLOCK_SECOND);
		obj_hash_ptr = obj_hash_with_prev;
		step = 1;
		break;
/*---------------------------------------------------------------------------*/
	case 5:
		// Close the JSON Object
case_5:
		memcpy(buffer, obj_end, sizeof(obj_end));
		len = sizeof(obj_end);
		step = 6;
		break;
	case 6:
		// One step to close the cycle
		step = 0;
		len = 0;
		break;
	default:
		for(;;);
	}

	if((len < 0) || (len >= buf_len)) {
		PRINTF("!!! Trying to send data bigger than the buffer or an enconding error has occurred!");
		len = 0;
	}

	return len;
}

/**
 * @brief Parses the type query param
 * @return size_t
 */
static size_t parse_type_str() {
	type_filter = strtoul(type_str, NULL, 10);
	return (type_filter > 0 || type_filter == UCHAR_MAX)? 1 : 0;
}

/**
 * @brief Common Handler on a HTTP request
 *
 * Takes a HTTP request and "files" a CoAP request
 *
 * @return nothing
 */
static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	http_response * rsp = (http_response *)response;
	httpd_state * req = (httpd_state *)request;

	/* -----------------  parse query parameters  ----------------- */
	type_len = REST.get_query_variable(request, TYPE_PARAM, &type_str);
	//
	PRINTF("** Type with len %d: ", type_len);
	int i; /* for PRINT_ARRAY */
	const char * ptr = type_str;
	PRINT_ARRAY(ptr, type_len);
	PRINTF("\n");
	/* Parameters are optional but need to be valid if present! */
	if(type_len > 0) {
		if((parse_type_str() == 0) || (type_len > TYPE_PARAM_MAX_LEN)) {
			PRINTF("!!! Invalid Type argument received!\n");
			set_http_error(request, error_invalid_params, REST.status.BAD_REQUEST);
			return;
		}
	}

	/* ----------------------------------------------------- */
	rsp->immediate_response = 1;
	rsp->large_rsp = RESPONSE_TYPE_LARGE;
	rsp->large_rsp_hnd = build_nodes_list;
    // Content-Type
    REST.set_header_content_type(req, REST.type.APPLICATION_JSON);
    // status code
    REST.set_response_status(req, REST.status.OK);
}
/*---------------------------------------------------------------------------*/
 
/**
* @}
*/
