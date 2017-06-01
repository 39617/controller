
#include "res-common.h"

static void res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE_HTTP(res_api,
          "title=\"HTTP API\";obs",
          res_get_handler,
          res_post_handler,
          NULL,
          NULL);


extern int coap_client_current_req_number;

static void
res_get_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
    rest_select_if(HTTP_IF);

    size_t len_value = 0;
    const char *target = NULL;
    const char *action = NULL;

    ((http_response *)response)->immediate_response = 0;
    REST.get_query_variable(request, "t", &target);
    len_value = REST.get_query_variable(request, "a", &action);

    PRINTF("t = %s | a = %s\n", target, action);

    int hash_val = atoi(target);
    int i;

    uint8_t* ip_addr = NULL;
    for(i = 0; i < 5; i++){
        if(online_coap_nodes_list[i].hash == hash_val){
            ip_addr = online_coap_nodes_list[i].ip;
        }
    }

    if(ip_addr == NULL)
        return; // TODO: return 404!
    ((httpd_state *)request)->action = (char*)action;
    ((httpd_state *)request)->action_len = len_value;
    ((httpd_state *)request)->dst_ipaddr = ip_addr;

    /* */
    if(coap_client_current_req_number < COAP_CLIENT_MAX_REQUESTS) {
        process_post_synch(&coap_client_process,
                           15, (process_data_t)request);
    } else {
        REST.set_response_status(response, 400);
        ((http_response *)response)->immediate_response = 1;
        ((http_response *)response)->status = http_header_404;
    }
}

static void
res_post_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{

}
