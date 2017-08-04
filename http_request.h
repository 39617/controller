/*
 * http_request.h
 *
 *  Created on: 27/07/2017
 *      Author: ricar
 */

#ifndef HTTP_REQUEST_H_
#define HTTP_REQUEST_H_

#include "contiki.h"

extern char host[32]; //!< Host of the server
extern uint16_t port; //!< Server port

extern process_event_t http_request_get_event;
extern process_event_t http_request_post_event;

#define PATH_MAX_SIZE           10 //!< max size of the path on the server
#define DATA_MAX_SIZE           512 //!< max size of data to send, webclient my accept different size!
//#define LIST_SIZE               2 //!< size of the list with pending requests
#define RUNNING                 0
#define FREE                    1

#define GET                     0
#define POST                    1

#define SUCCESS                 0
#define GENERIC_ERROR           -1

/// Struct used for webclient requests
typedef struct _http_request {
    //size_t action; //!< HTTP Action
    //char host[32]; //!< Host of the server
    //uint16_t port; //!< Server port
    char *path; //!< path for the resource
    char *data; //!< Pointer to data
    //char path[PATH_MAX_SIZE]; //!< path for the resource
    //char data[DATA_MAX_SIZE]; //!< Pointer to data
    void (*callback)(char *data, uint16_t len, int identifier, int state); //!< Called when response is received, state indicate success or error
    int identifier; //!< identifier of hardware that requested this request
}http_request_t, *p_http_request_t;


/// Struct used for the webclient requests list
/*typedef struct _http_request_list {
  struct *next; //!< Pointer to next member in the struct
  http_request_t item; //!< http_request struct
}http_request_list_t, *p_http_request_list_t;*/
PROCESS_NAME(http_request_process);


#endif /* HTTP_REQUEST_H_ */
