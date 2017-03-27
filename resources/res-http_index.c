
#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "er-http.h"
#include "parser.h"

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

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);


RESOURCE_HTTP(res_http_index,
		  "title=\"HTTP Index\";obs",
		  res_get_handler,
		  res_post_handler,
		  NULL,
		  NULL);


static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

	//const char *len = NULL;
	/* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
	const char *message = "{\"Tomada1\" : \"30Amperes\"}";
	int length = 60;

	REST.set_response_payload(request, message, length);
	REST.set_header_content_type(request, REST.type.APPLICATION_JSON);
	REST.set_response_status(request, REST.status.OK);
}

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	http_packet_t *packet = (http_packet_t *)request;
	PRINTF("POST PAYLOAD: %s\n", packet->buffer);
	parse_pair_t key_pair[2];
	//char key[2][200];
	//char value[2][200];

	parse_post_param(packet, key_pair, 2);

	//PRINTF("PARAM2 = %s - VAL = %s\n", key[0], value[0]);

	const char *message = "{\"POST\" : \"PAYLOAD\"}";
	int length = 21;
	REST.set_response_payload(request, message, length);
	REST.set_header_content_type(request, REST.type.APPLICATION_JSON);
	REST.set_response_status(request, REST.status.OK);
}



