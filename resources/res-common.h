/*
 * res-common.h
 *
 * Common includes for all resources.
 */

#ifndef RESOURCES_RES_COMMON_H_
#define RESOURCES_RES_COMMON_H_

#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "er-http.h"
#include "parser.h"
#include "node-table.h"
#include "error_codes.h"
#include "er-coap-engine.h"
#include "coap_client.h"

/*---------------------------------------------------------------------------*/
#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#define PRINT_ARRAY(str, len) \
	do { \
		for(i = 0; i < len; i++) { \
			PRINTF("%c", *(str)); \
			(str) = ((str) + 1); \
		} \
	} \
	while(0)
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#define PRINT_ARRAY(str, len)
#endif
/*---------------------------------------------------------------------------*/

#endif /* RESOURCES_RES_COMMON_H_ */
