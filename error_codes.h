/*
 * error_codes.h
 *
 */

#ifndef ERROR_CODES_H_
#define ERROR_CODES_H_

#define ERROR_BUFFER_SIZE                     16

extern char error_buffer[ERROR_BUFFER_SIZE];
extern char error_template[];

/* Invalid parameters */
extern char error_invalid_params[];
/* Node offline */
extern char error_node_not_found[];
/* Max requests per node reached */
extern char error_node_unavailable[];
/* Too Many Requests */
extern char error_too_many_requests[];

#endif /* ERROR_CODES_H_ */
