
#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(res_http_index,
		  "title=\"HTTP Index\";obs",
		  res_get_handler,
		  res_post_handler,
		  NULL,
		  NULL);


static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

	const char *len = NULL;
	/* Some data that has the length up to REST_MAX_CHUNK_SIZE. For more, see the chunk resource. */
	const char *message = "HTTTP Index static text\n";
	int length = 24;

	REST.set_response_payload(response, message, length);
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN); /* text/plain is the default, hence this option could be omitted. */
}

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
	REST.set_response_status(response, REST.status.OK);
}



