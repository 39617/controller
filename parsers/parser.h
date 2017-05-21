/*
 * parser.h
 *
 *  Created on: May 17, 2017
 *      Author: user
 */

#ifndef EXAMPLES_HTTP_COAP_PARSERS_PARSER_H_
#define EXAMPLES_HTTP_COAP_PARSERS_PARSER_H_

#include "er-http.h"

#define ISO_nl        0x0A
#define ISO_space     0x20
#define ISO_slash     0x2F
#define ISO_amp       0x26
#define ISO_column    0x3A
#define ISO_equal     0x3D

#define HEX_TO_INT(x)  (isdigit(x) ? x - '0' : x - 'W')

typedef struct parse_pair{
    char key[255];
    char value[255];
} parse_pair_t, *p_parse_pair_t;

void parse_post_param(httpd_state *s, p_parse_pair_t key_pair, int len_user);

#endif /* EXAMPLES_HTTP_COAP_PARSERS_PARSER_H_ */
