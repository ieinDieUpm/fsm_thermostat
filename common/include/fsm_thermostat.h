/**
 * @file fsm_thermostat.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the thermostat FSM.
 * @date 2024-05-01
 *
 */

#ifndef FSM_THERMOSTAT_H
#define FSM_THERMOSTAT_H

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>

/* Other includes */
#include <fsm.h>
#include "port_led.h"
#include "port_temp_sensor.h"

/* Defines and enums ----------------------------------------------------------*/
/* Defines */
#define THERMOSTAT_TIMEOUT_SEC 1        /*!< Timeout for the thermostat to be activated */
#define THERMOSTAT_HISTORY 10           /*!< Number of events to store in the thermostat */
#define THERMOSTAT_DEFAULT_THRESHOLD 25 /*!< Threshold temperature to activate the thermostat */

/* Enums */
/**
 * @brief Enumerates the states of the thermostat FSM.
 *
 */
enum FSM_THERMOSTAT_STATES
{
    THERMOSTAT_OFF = 0, /*!< Thermostat is off */
    THERMOSTAT_ON       /*!< Thermostat is on */
};

/**
 * @brief Enumerates the events of the thermostat FSM.
 *
 */
enum THERMOSTAT_EVENTS
{
    UNKNOWN = -1,   /*!< Unknown event */
    ACTIVATION = 0, /*!< Thermostat activated */
    DEACTIVATION    /*!< Thermostat deactivated */
};

/* Typedefs ------------------------------------------------------------------*/
typedef struct fsm_thermostat_t fsm_thermostat_t; /*!< Declaration of the FSM structure */

/* Function prototypes and explanations ---------------------------------------*/
/**
 * @brief Creates a new thermostat FSM.
 *
 * @param led_heat_id ID of the heating LED.
 * @param led_comfort_id ID of the comfort LED.
 * @param temp_sensor_id ID of the temperature sensor.
 * @return fsm_thermostat_t* Pointer to the new thermostat FSM.
 */
fsm_thermostat_t *fsm_thermostat_new(uint32_t led_heat_id, uint32_t led_comfort_id, uint32_t temp_sensor_id);

/**
 * @brief Gets the last time there was an event in the thermostat. If the event is not found, it returns 0.
 *
 * @param p_this Pointer to the thermostat FSM structure.
 * @param event Event to check. It can be any of the events in the THERMOSTAT_EVENTS enum.
 */
uint32_t fsm_thermostat_get_last_time_event(fsm_thermostat_t *p_this, uint8_t event);

/**
 * @brief Gets the thermostat status.
 *
 * @param p_this Pointer to the thermostat FSM structure.
 */
uint8_t fsm_thermostat_get_status(fsm_thermostat_t *p_this);

/**
 * @brief Fires the thermostat FSM.
 * 
 * @param p_fsm Pointer to the thermostat FSM structure. 
 */
void fsm_thermostat_fire(fsm_thermostat_t *p_fsm);

#endif /* FSM_THERMOSTAT_H */