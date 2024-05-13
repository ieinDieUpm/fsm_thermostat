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
/**
 * @brief Structure to define the thermostat FSM.
 */
typedef struct
{
    fsm_t f;                                       /*!< FSM structure. Important to be the first element of the structure */
    port_led_hw_t *p_led_heat;                     /*!< Pointer to the heat LED structure */
    port_led_hw_t *p_led_comfort;                  /*!< Pointer to the cool LED structure */
    port_temp_hw_t *p_temp_sensor;                 /*!< Pointer to the temperature sensor structure */
    bool last_events[THERMOSTAT_HISTORY];          /*!< Statuses of the thermostat */
    uint32_t last_time_events[THERMOSTAT_HISTORY]; /*!< Last times of events detected */
    uint8_t event_idx;                             /*!< Index of the last event */
    double threshold_temp_celsius;                 /*!< Threshold temperature to activate the thermostat Celsius */
    uint32_t timer_period_sec;                     /*!< Period of the timer to measure the temperature */
} fsm_thermostat_t;

/* Function prototypes and explanations ---------------------------------------*/
/**
 * @brief Creates a new thermostat FSM.
 *
 * @param p_led_heat Pointer to the LED of the thermostat.
 * @param p_led_comfort Pointer to the comfort LED of the thermostat.
 * @param p_temp Pointer to the temperature sensor of the thermostat.
 * @return fsm_thermostat_t* Pointer to the new thermostat FSM.
 */
fsm_t *fsm_thermostat_new(port_led_hw_t *p_led_heat, port_led_hw_t *p_led_comfort, port_temp_hw_t *p_temp);

/**
 * @brief Gets the last time there was an event in the thermostat. If the event is not found, it returns 0.
 *
 * @param p_this Pointer to the thermostat FSM structure.
 * @param event Event to check. It can be any of the events in the THERMOSTAT_EVENTS enum.
 */
uint32_t fsm_thermostat_get_last_time_event(fsm_t *p_this, uint8_t event);

/**
 * @brief Gets the thermostat status.
 *
 * @param p_this Pointer to the thermostat FSM structure.
 */
uint8_t fsm_thermostat_get_status(fsm_t *p_this);

#endif /* FSM_THERMOSTAT_H */