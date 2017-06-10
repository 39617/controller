/**
 * @file     parser.h
 * @brief    Contains the parser definitions
 * @version  1.0
 * @date     01 Jun. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#ifndef EXAMPLES_HTTP_COAP_PARSERS_PARSER_H_
#define EXAMPLES_HTTP_COAP_PARSERS_PARSER_H_

/** @addtogroup Parser Parser
* @{
*/
#include <stddef.h>

#include "er-http.h"
#include "httpd.h"

/* POST request machine states */
#define PARSE_POST_STATE_INIT            0 /*!< Initial State of Parser */
#define PARSE_POST_STATE_MORE            1 /*!< Found More State */
#define PARSE_POST_STATE_READING_KEY     2 /*!< Parsing Key */
#define PARSE_POST_STATE_READING_VAL     3 /*!< Parsing Val */
#define PARSE_POST_STATE_OK              0x4 /*!< Parsing OK */
#define PARSE_POST_STATE_ERROR           0xFFFFFFFF /*!< Error on Parsing */

#define HEX_TO_INT(x)  (isdigit(x) ? x - '0' : x - 'W')

typedef struct parse_pair
{
    char key[POST_PARAMS_KEY_MAX_LEN]; /*!< Stores the Key */
    char value[POST_PARAMS_VAL_MAX_LEN]; /*!< Stores the Val */
} parse_pair_t, *p_parse_pair_t;

/**
 * @brief Parses the query/post body
 *
 * Stores the result on a struct key_pair
 *
 * @param buf : The buffer to parse
 * @param buf_len : The size of the buffer
 * @param key_pair : The destination of the result
 * @param len_user : The number of params to parse
 * @return size_t
 */
size_t parse_post_param(char *buf, int buf_len, p_parse_pair_t key_pair,
                        int len_user);

/**
 * @}
 */

#endif
