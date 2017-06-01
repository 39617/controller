/*
 * error_codes.c
 *
 *  Defenition of all error used for the API REST
 */

#include "error_codes.h"


char error_buffer[ERROR_BUFFER_SIZE];
char error_template[] = "{\"error\":\"%s\"}";

/* Invalid parameters */
char error_invalid_params[] =           "0x1";
/* Node offline */
char error_node_not_found[] =           "0x2";
/* Max requests per node reached */
char error_node_unavailable[] =         "0x3";
/* Too Many Requests */
char error_too_many_requests[] =        "0x4";
