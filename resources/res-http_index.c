
#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "er-http.h"
#include "parser.h"

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
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
#define PRINT_ARRAY(...)
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
	httpd_state *packet = (httpd_state *)request;
	//PRINTF("POST PAYLOAD: %s\n", packet->buffer);
#if DEBUG
	PRINTF("* POST PAYLOAD\n");
	char * ptr = (char *)&packet->buffer;
	int i;
	PRINT_ARRAY(ptr, packet->content_length);
	PRINTF("\n");
#endif

	parse_pair_t key_pair[2];
	size_t ret = parse_post_param(packet->buffer, packet->content_length, (p_parse_pair_t)&key_pair, 2);
	if(ret == PARSE_POST_STATE_ERROR) {
		REST.set_header_content_type(request, REST.type.TEXT_HTML);
		REST.set_response_status(request, REST.status.BAD_REQUEST);
		return;
	}

#if DEBUG
	PRINTF("Key.0: %s", key_pair[0].key);
	PRINTF("Value.0: %s", key_pair[0].value);
	PRINTF("Key.1: %s", key_pair[1].key);
	PRINTF("Value.1: %s", key_pair[1].value);
//	ptr = (char *)&key_pair[0].key;
//	PRINTF("Key.0: ");
//	PRINT_ARRAY(ptr, 10);
//	PRINTF(" ** ");
//	ptr = (char *)&key_pair[0].value;
//	PRINTF("Value.0: ");
//	PRINT_ARRAY(ptr, 10);
//	//
//	ptr = (char *)&key_pair[1].key;
//	PRINTF("\nKey.1: ");
//	PRINT_ARRAY(ptr, 10);
//	PRINTF(" ** ");
//	ptr = (char *)&key_pair[1].value;
//	PRINTF("Value.1: ");
//	PRINT_ARRAY(ptr, 10);
//	//
//	PRINTF("\n");
#endif

//	const char *message = "{\"POST\" : \"PAYLOAD\"}";
//	int length = 21;
//	REST.set_response_payload(request, message, length);
	REST.set_header_content_type(request, REST.type.APPLICATION_JSON);
	REST.set_response_status(request, REST.status.OK);
}



