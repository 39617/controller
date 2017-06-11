//TODO TO BE DELETED


#include "res-common.h"

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


// NOTA: It MUST end with NULL
const char *dummy_hdr[] = {"Dummy: Header\r\n", NULL};
static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	httpd_state *packet = (httpd_state *)request;

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
		http_set_redir_path(request, "/index2"); /* propositado para dar not found */
		return;
	}

#if DEBUG
	PRINTF("Key.0: %s", key_pair[0].key);
	PRINTF(" * Value.0: %s\n", key_pair[0].value);
	PRINTF("Key.1: %s", key_pair[1].key);
	PRINTF(" * Value.1: %s\n", key_pair[1].value);
#endif


	REST.set_response_status(request, REST.status.OK);
	REST.set_header_content_type(request, REST.type.APPLICATION_JSON);
	http_set_redir_path(request, "/index");
	http_set_additional_headers(request, (const void **)&dummy_hdr);
}



