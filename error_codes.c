/**
 * @file     error_codes.c
 * @brief    Contains the information error used for the API REST
 * @version  1.0
 * @date     01 Jun. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#include "error_codes.h"

char error_buffer[ERROR_BUFFER_SIZE]; /*!< Error Buffer */
char error_template[] = "{\"error\":\"%s\"}"; /*!< Template */

char error_invalid_params[] =           "0x1"; /*!< Invalid parameters */
char error_node_not_found[] =           "0x2"; /*!< Node offline */
char error_node_unavailable[] =         "0x3"; /*!< Max requests per node reached */
char error_too_many_requests[] =        "0x4"; /*!< Too Many Requests */
