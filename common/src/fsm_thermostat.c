/**
 * @file fsm_thermostat.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Finite State Machine for a thermostat.
 * @date 2024-05-01
 *
 */

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdlib.h>
#include <string.h>

/* Project includes */
#include "fsm_thermostat.h"
#include "port_thermostat.h"
#include "port_led.h"
#include "port_temp_sensor.h"

/* State machine input or transition functions */

/**
 * @brief Check if the temperature is cold enough to activate the thermostat
 *
 * @param p_this Pointer to the FSM structure
 * @return true if the temperature is cold, false otherwise
 */
bool check_heat(fsm_t *p_this)
{
    // Retrieve the FSM structure and get the temperature sensor
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)p_this;

    // Get the temperature from the sensor
    if (port_temp_sensor_get_temperature(p_fsm->p_temp_sensor) < p_fsm->threshold_temp_celsius)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief Check if the temperature is hot enough to deactivate the thermostat
 *
 * @param p_this Pointer to the FSM structure
 * @return true if the temperature is hot, false otherwise
 */
bool check_comfort(fsm_t *p_this)
{
    return !check_heat(p_this);
}

/* State machine output or action functions */

/**
 * @brief Turn off the thermostat
 *
 * @param p_this Pointer to the thermostat FSM structure
 */
void do_thermostat_on(fsm_t *p_this)
{
    // Retrieve the FSM structure and get the LED
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)p_this;

    // Set the LEDs according to the thermostat status
    port_led_on(p_fsm->p_led_heat);
    port_led_off(p_fsm->p_led_comfort);

    // Store the event
    p_fsm->last_events[p_fsm->event_idx] = ACTIVATION;
    p_fsm->last_time_events[p_fsm->event_idx] = port_system_get_millis();
    p_fsm->event_idx = (p_fsm->event_idx + 1) % THERMOSTAT_HISTORY;
}

/**
 * @brief Turn on the thermostat
 *
 * @param p_this Pointer to the thermostat FSM structure
 */
void do_thermostat_off(fsm_t *p_this)
{
    // Retrieve the FSM structure and get the LED
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)p_this;

    // Set the LEDs according to the thermostat status
    port_led_off(p_fsm->p_led_heat);
    port_led_on(p_fsm->p_led_comfort);

    // Store the event
    p_fsm->last_events[p_fsm->event_idx] = DEACTIVATION;
    p_fsm->last_time_events[p_fsm->event_idx] = port_system_get_millis();
    p_fsm->event_idx = (p_fsm->event_idx + 1) % THERMOSTAT_HISTORY;
}

/* Transitions table ---------------------------------------------------------*/
/**
 * @brief Transitions table for the thermostat
 *
 */
fsm_trans_t fsm_trans_thermostat[] = {
    {THERMOSTAT_OFF, check_heat, THERMOSTAT_ON, do_thermostat_on},
    {THERMOSTAT_ON, check_comfort, THERMOSTAT_OFF, do_thermostat_off},
    {-1, NULL, -1, NULL},
};

uint32_t fsm_thermostat_get_last_time_event(fsm_t *p_this, uint8_t event)
{
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)p_this;

    // Look for the last time the event was detected, if it was detected. If not, return 0
    for (int i = 0; i < THERMOSTAT_HISTORY; i++)
    {
        if (p_fsm->last_events[i] == event)
        {
            return p_fsm->last_time_events[i];
        }
    }
    return 0;
}

uint8_t fsm_thermostat_get_status(fsm_t *p_this)
{
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)p_this;
    return p_fsm->last_events[p_fsm->event_idx];
}

/* Initialize the FSM */

/**
 * @brief Initialize the thermostat FSM
 *
 * @param p_this Pointer to the FSM structure
 * @param p_led_heat Pointer to the LED structure
 * @param p_led_comfort Pointer to the LED structure
 * @param p_temp Pointer to the temperature sensor structure
 */
void fsm_thermostat_init(fsm_t *p_this, port_led_hw_t *p_led_heat, port_led_hw_t *p_led_comfort, port_temp_hw_t *p_temp)
{
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)(p_this);
    fsm_init(p_this, fsm_trans_thermostat);

    // Assign the peripherals to the FSM
    p_fsm->p_led_heat = p_led_heat;
    p_fsm->p_led_comfort = p_led_comfort;
    p_fsm->p_temp_sensor = p_temp;

    // Initialize the last time the thermostat was activated
    memset(p_fsm->last_time_events, 0, sizeof(p_fsm->last_time_events));

    // Initialize the thermostat status
    memset(p_fsm->last_events, UNKNOWN, sizeof(p_fsm->last_events));

    // Initialize the event index
    p_fsm->event_idx = 0;

    // Initialize the threshold temperature
    p_fsm->threshold_temp_celsius = THERMOSTAT_DEFAULT_THRESHOLD;

    // Initialize the timer to measure the temperature
    p_fsm->timer_period_sec = THERMOSTAT_TIMEOUT_SEC;

    // Initialize the timer
    port_thermostat_timer_setup(p_fsm);

    // Initialize the peripherals
    port_led_init(p_led_heat);
    port_led_init(p_led_comfort);    
    port_temp_sensor_init(p_temp);
}

/* Create FSM */
fsm_t *fsm_thermostat_new(port_led_hw_t *p_led_heat, port_led_hw_t *p_led_comfort, port_temp_hw_t *p_temp)
{
    // Do malloc for the whole FSM structure to reserve memory for the rest of the FSM, although I interpret it as fsm_t which is the first field of the structure so that the FSM library can work with it
    fsm_t *p_fsm = malloc(sizeof(fsm_thermostat_t));

    // Initialize the FSM
    fsm_thermostat_init(p_fsm, p_led_heat, p_led_comfort, p_temp);

    return p_fsm;
}
