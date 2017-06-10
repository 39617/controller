/**
 * @file     controller.h
 * @brief    Contains the Controller definitions
 * @version  1.0
 * @date     01 Jun. 2017
 * @author   Tiago Costa & Ricardo Jesus & Claudio Prates
 *
 **/

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

/** @defgroup Controller Controller
 * @{
 */

#include "contiki.h"

extern uint16_t online_nodes_counter; /*!< Total number of online nodes, public exposed */

PROCESS_NAME(controller_process);

/**
 * @}
 */

#endif /* CONTROLLER_H_ */
