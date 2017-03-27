
#include <stdlib.h>
#include <string.h>
#include "rest-engine.h"
#include "er-http.h"
#include "parser.h"
#include "controller.h"
#include "er-coap-engine.h"
#include "coap_client.h"

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

RESOURCE_HTTP(res_api,
          "title=\"HTTP API\";obs",
          res_get_handler,
          res_post_handler,
          NULL,
          NULL);


extern address_table_t addr_table[50];
extern int coap_client_current_req_number;

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    rest_select_if(HTTP_IF);

    size_t len_value = 0;
    const char *target = NULL;
    const char *action = NULL;

    ((http_response *)response)->imediate_response = 0;
    REST.get_query_variable(request, "t", &target);
    len_value = REST.get_query_variable(request, "a", &action);

    PRINTF("t = %s | a = %s\n", target, action);

    int hash_val = atoi(target);
    int i;

    char* ip_addr = NULL;
    for(i = 0; i < 50; i++){
        if(addr_table[i].hash == hash_val){
            ip_addr = addr_table[i].ip;
        }
    }

    if(ip_addr == NULL)
        return; // TODO: return 404!
    ((httpd_state *)request)->action = (char*)action;
    ((httpd_state *)request)->action_len = len_value;
    ((httpd_state *)request)->dst_ipaddr = ip_addr;

    /* */
    if(coap_client_current_req_number < MAX_REQUEST_COAP_CLIENT) {
        process_post_synch(&coap_client_process,
                           15, (process_data_t)request);
    } else {
        REST.set_response_status(response, 400);
        ((http_response *)response)->imediate_response = 1;
        ((http_response *)response)->status = http_header_404;
    }
}

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

}
