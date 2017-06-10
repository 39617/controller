
#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
//#include "jsontree.h"
#include "er-http.h"
#include "jsonutils.h"

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
	//const char *message = "{\"Tomada1\" : \"30Amperes\"}";

	/*static struct jsontree_string tomada = JSONTREE_STRING("Tomada1");
	static struct jsontree_string desc = JSONTREE_STRING("30Amperes");
	JSONTREE_OBJECT(tomada_tree,
	                JSONTREE_PAIR(&tomada, &desc),
					JSONTREE_PAIR("Tomada2", "40Amperes"));
	char buf[70];
	struct jsontree_context json;

	int pos = 0;
	int length = sizeof(buf);

	json.values[0] = (struct json_objects)tomada_tree;
	jsontree_reset(&json);
	find_json_path(&json, "");
	json.path = json.depth;
	//json.putchar = putchar_udp;
	while(jsontree_print_next(&json) && json.path <= json.depth);

	printf("Real UDP size: %d\n", pos);
	buf[pos] = 0;*/

	/*const char *message = "{\"Tomada1\" : \"30Amperes\"}";
	int length = 60;
	struct jsonparse_state parserState;
	jsonparse_setup(&parserState, message, length);
	//jsonparse_next(&parserState);
	char buf[70];
	int sizeBuf = 70;
	int coiso;
	//int coiso = jsonparse_copy_value(&parserState, &buf,sizeBuf);
	printf("inicio testes // struct %s\n", parserState.json);
	//printf("%s", buf);
	coiso = jsonparse_next(&parserState);
	printf("resultado next %d \n", coiso);
	coiso = jsonparse_copy_value(&parserState, &buf,
			sizeBuf);
	printf("resultado copy %d \n", coiso);
	printf("temos no buf %s \n", %buf);
	printf("fim testes \n");*/

    /*unsigned int accept = -1;
  	REST.get_header_accept(request, &accept);
  	if(accept == REST.type.APPLICATION_JSON) {
  		REST.set_response_payload(response, "hmmm", 70);
  		REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
  	}else{
		REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
		const char *msg = "Supporting content-types application/json";
		REST.set_response_payload(response, msg, strlen(msg));
  	}*/
	struct jsonparse_state parserState;
	const char *message = "{\"Tomada1\":3,\"Tomada2\":6}";
	int length = 25;
	jsonparse_setup(&parserState, message, length);
	//jsonparse_next(&parserState);
	char buf[length];
	int sizeBuf = length;
	int parseStatus;
	//int coiso = jsonparse_copy_value(&parserState, &buf,sizeBuf);
	printf("inicio testes // struct %s\n", parserState.json);
	//printf("%s", buf);
	int fdx = 0;
	jsonparse_search(&parserState, "Tomada2");
	jsonparse_search(&parserState, "Tomada1");
	//while(fdx < 15){

		//fdx++;
	//}

	/*while((parseStatus = jsonparse_next(&parserState)) != 0 || parseStatus != JSON_TYPE_ERROR){
		printf("parseStatus atual %c\n",parseStatus );

		if(jsonparse_copy_value(&parserState, &buf,sizeBuf) != 0)
			printf("temos no buf %s \n", buf);


		printf("depth %d \n",parserState.depth);
		if(parserState.depth == 0)
			break;

	}
	if(parseStatus != JSON_TYPE_ERROR)
		printf("json invalido\n");
	printf("valores estrutura depois dos nexts todos \n");

	printf("json %s \n", parserState.json);
	printf("pos %d \n", parserState.pos);
	printf("len %d \n", parserState.len);
	printf("depth %d \n", parserState.depth);
	printf("vstart %d \n", parserState.vstart);
	printf("vlen %d \n", parserState.vlen);
	printf("vtype %c \n", parserState.vtype);
	printf("error %c \n", parserState.error);*/


	  /*const char *json;
	  int pos;
	  int len;
	  int depth;

	  int vstart;
	  int vlen;
	  char vtype;
	  char error;
	  char stack[JSONPARSE_MAX_DEPTH];
	  */


	/*if(coiso == JSON_TYPE_OBJECT)
		printf("%c\n", coiso = jsonparse_next(&parserState));
	if(coiso == JSON_TYPE_PAIR_NAME)
		printf("%c\n", coiso = jsonparse_next(&parserState));*/




	/*int coiso;
	coiso = jsonparse_copy_value(&parserState, &buf,
			sizeBuf);




	printf("resultado copy %d \n", coiso);
	printf("temos no buf final %s \n", buf);*/
	printf("fim testes \n");


	REST.set_response_payload(response, "get method", 70);
  	REST.set_header_content_type(response, REST.type.APPLICATION_JSON); /* text/plain is the default, hence this option could be omitted.*/

}

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	printf("post detected \n");
	unsigned int contentType = -1;
	//REST.get_header_content_type(request, &contentType);
	REST.set_header_content_type(response, REST.type.APPLICATION_JSON);
	printf("content-type %d \n", contentType);
	struct httpd_state * const http_req = (struct httpd_state *)request;
	//if(request-> == REST.type.APPLICATION_JSON) {
		printf("json content type \n");



		REST.set_response_status(response, REST.status.OK);
	/*}else
	{
		REST.set_response_status(response, REST.status.BAD_REQUEST);
	    const char *error_msg = "Unsupported content-type";
	    REST.set_response_payload(response, error_msg, strlen(error_msg));
	}*/

}


