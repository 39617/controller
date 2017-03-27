/*
 * parser.h
 *
 *  Created on: May 17, 2017
 *      Author: user
 */

#ifndef EXAMPLES_HTTP_COAP_PARSERS_PARSER_H_
#define EXAMPLES_HTTP_COAP_PARSERS_PARSER_H_

#include <stddef.h>

#include "er-http.h"
#include "httpd.h"

/* POST request machine states */
#define PARSE_POST_STATE_INIT            0
#define PARSE_POST_STATE_MORE            1
#define PARSE_POST_STATE_READING_KEY     2
#define PARSE_POST_STATE_READING_VAL     3
#define PARSE_POST_STATE_ERROR           0xFFFFFFFF


#define HEX_TO_INT(x)  (isdigit(x) ? x - '0' : x - 'W')

typedef struct parse_pair{
    char key[POST_PARAMS_KEY_MAX_LEN];
    char value[POST_PARAMS_VAL_MAX_LEN];
} parse_pair_t, *p_parse_pair_t;


size_t parse_post_param(char *buf, int buf_len, p_parse_pair_t key_pair, int len_user);

#endif /* EXAMPLES_HTTP_COAP_PARSERS_PARSER_H_ */
