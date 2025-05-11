/**
 * @file port_thermostat.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the thermostat system port layer.
 * @version 0.1
 * @date 2025-05-01
 *
 */

#ifndef PORT_THERMOSTAT_H
#define PORT_THERMOSTAT_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Defines and macros --------------------------------------------------------*/
#define PORT_THERMOSTAT_ID 0 /*!< Thermostat identifier @hideinitializer */

/**
 * @brief Initializes the timer of the thermostat.
 *
 * @param thermostat_id Thermostat ID.
 */
void port_thermostat_timer_setup(uint32_t thermostat_id);

#endif
