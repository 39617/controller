#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "contiki.h"

typedef struct _address_table{
    int hash;
    char mac[8];
    char ip[16];
} address_table_t, p_address_table_t;

PROCESS_NAME(controller_process);

#endif /* CONTROLLER_H_ */
