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
#include "coap_node.h"

/*---------------------------------------------------------------------------*/
static void
res_common_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

#define MAX_PATH_SIZE   30
#define MAX_DATA_SIZE   200
typedef struct _coap_http_request
{
    char path[MAX_PATH_SIZE]; //!< path for the resource
    char data[MAX_DATA_SIZE]; //!< Pointer to data
    int identifier; //!< identifier of the device - requesting device
} coap_http_request_t, *p_coap_http_request_t;
coap_http_request_t local_req;

http_request_t current_req;
int state;

/*---------------------------------------------------------------------------*/

RESOURCE(res_coaptohttp,
          "CoAP to HTTP Node",
		  res_get_handler,
		  res_post_handler,
		  res_common_handler,
		  res_common_handler); /*!< Creates a resource_t and marks it as CoAP */


static void get_callback(char *data, uint16_t len, int identifier, int state){
    PRINTF("GET Callback called \n");
    if(state < 0 ){
        PRINTF("GET ERROR: %s \n", data);
    }else{
        PRINTF("Received data: %s \n", data);
    }
}

/**
 * @brief Common Handler on a CoAP request
 *
 * For methods not implemented
 *
 * @return nothing
 */
static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    if (state == 0){
        state = 1;
        PRINTF("GET");
        coap_http_request_t req = (coap_http_request_t *) request;
        current_req.callback = get_callback;
        current_req.identifier = req.identifier;
        current_req.path = req.path;
        current_req.data = req.data;
        process_post_synch(&http_request_process, http_request_get_event, &current_req);
        REST.set_response_status(response, CONTENT_2_05);
    }
    else {
        REST.set_response_status(response, SERVICE_UNAVAILABLE_5_03);
    }

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
    PRINTF("POST");
}



/**
 * @brief Common Handler on a CoAP request
 *
 * For methods not implemented
 *
 * @return nothing
 */
static void
res_common_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    REST.set_response_status(response, METHOD_NOT_ALLOWED_4_05);
}

/**
 * @}
 */
 
/**
* @}
*/
