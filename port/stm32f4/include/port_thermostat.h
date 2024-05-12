/**
 * @file port_thermostat.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the thermostat system port layer.
 * @version 0.1
 * @date 2024-05-01
 *
 */

#ifndef PORT_THERMOSTAT_H
#define PORT_THERMOSTAT_H

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */

/* HW dependent includes */
#include "port_system.h"
#include "fsm_thermostat.h"

/* Defines and macros --------------------------------------------------------*/
// HW Nucleo-STM32F446RE:
#define THERMOSTAT_MEASUREMENT_TIMER TIM2 /*!< Timer to measure the temperature */

/**
 * @brief Initializes the timer of the thermostat.
 *
 * @param thermostat Pointer to the thermostat structure.
 */
void port_thermostat_timer_setup(fsm_thermostat_t *p_thermostat);

#endif
