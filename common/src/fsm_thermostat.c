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
#include "port_system.h"

/**
 * @brief Structure to define the thermostat FSM.
 */
struct fsm_thermostat_t
{
    fsm_t f;                                       /*!< FSM structure. Important to be the first element of the structure */
    uint32_t led_heat_id;                          /*!< ID of the heating LED */
    uint32_t led_comfort_id;                       /*!< ID of the comfort LED */
    uint32_t temp_sensor_id;                       /*!< ID of the temperature sensor */
    bool last_events[THERMOSTAT_HISTORY];          /*!< Statuses of the thermostat */
    uint32_t last_time_events[THERMOSTAT_HISTORY]; /*!< Last times of events detected */
    uint8_t event_idx;                             /*!< Index of the last event */
    double threshold_temp_celsius;                 /*!< Threshold temperature to activate the thermostat Celsius */
    uint32_t timer_period_sec;                     /*!< Period of the timer to measure the temperature */
};

/* State machine input or transition functions */

/**
 * @brief Check if the temperature is cold enough to activate the thermostat
 *
 * @param p_fsm_thermostat Pointer to the FSM structure
 * @return true if the temperature is cold, false otherwise
 */
bool check_heat(fsm_t *p_fsm_thermostat)
{
    // Retrieve the FSM structure and get the temperature sensor
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)p_fsm_thermostat;

    // Get the temperature from the sensor
    if (port_temp_sensor_get_temperature(p_fsm->temp_sensor_id) < p_fsm->threshold_temp_celsius)
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
 * @param p_fsm_thermostat Pointer to the FSM structure
 * @return true if the temperature is hot, false otherwise
 */
bool check_comfort(fsm_t *p_fsm_thermostat)
{
    return !check_heat(p_fsm_thermostat);
}

/* State machine output or action functions */

/**
 * @brief Turn off the thermostat
 *
 * @param p_fsm_thermostat Pointer to the thermostat FSM structure
 */
void do_thermostat_on(fsm_t *p_fsm_thermostat)
{
    // Retrieve the FSM structure and get the LED
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)p_fsm_thermostat;

    // Set the LEDs according to the thermostat status
    port_led_set_value(p_fsm->led_heat_id, true);
    port_led_set_value(p_fsm->led_comfort_id, false);

    // Store the event
    p_fsm->last_events[p_fsm->event_idx] = ACTIVATION;
    p_fsm->last_time_events[p_fsm->event_idx] = port_system_get_millis();
    p_fsm->event_idx = (p_fsm->event_idx + 1) % THERMOSTAT_HISTORY;
}

/**
 * @brief Turn on the thermostat
 *
 * @param p_fsm_thermostat Pointer to the thermostat FSM structure
 */
void do_thermostat_off(fsm_t *p_fsm_thermostat)
{
    // Retrieve the FSM structure and get the LED
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)p_fsm_thermostat;

    // Set the LEDs according to the thermostat status
    port_led_set_value(p_fsm->led_heat_id, false);
    port_led_set_value(p_fsm->led_comfort_id, true);

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

uint32_t fsm_thermostat_get_last_time_event(fsm_thermostat_t *p_fsm, uint8_t event)
{
    // Return the last event detected
    if (p_fsm->event_idx == 0)
    {
        return p_fsm->last_time_events[THERMOSTAT_HISTORY - 1];
    }
    else
    {
        return p_fsm->last_time_events[p_fsm->event_idx - 1];
    }
}

uint8_t fsm_thermostat_get_status(fsm_thermostat_t *p_fsm)
{
    // Return the last event detected
    if (p_fsm->event_idx == 0)
    {
        return p_fsm->last_events[THERMOSTAT_HISTORY - 1];
    }
    else
    {
        return p_fsm->last_events[p_fsm->event_idx - 1];
    }
}

/* Initialize the FSM */

/**
 * @brief Initialize the thermostat FSM
 *
 * @param p_fsm_thermostat Pointer to the FSM structure
 * @param p_led_heat Pointer to the LED structure
 * @param p_led_comfort Pointer to the LED structure
 * @param p_temp Pointer to the temperature sensor structure
 */
void fsm_thermostat_init(fsm_thermostat_t *p_fsm_thermostat, uint32_t led_heat_id, uint32_t led_comfort_id, uint32_t temp_sensor_id)
{
    fsm_thermostat_t *p_fsm = (fsm_thermostat_t *)(p_fsm_thermostat);
    fsm_init(&p_fsm_thermostat->f, fsm_trans_thermostat);

    // Assign the peripherals to the FSM
    p_fsm->led_heat_id = led_heat_id;
    p_fsm->led_comfort_id = led_comfort_id;
    p_fsm->temp_sensor_id = temp_sensor_id;

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
    port_thermostat_timer_setup(PORT_THERMOSTAT_ID);

    // Initialize the peripherals
    port_led_init(led_heat_id);
    port_led_init(led_comfort_id);
    port_temp_sensor_init(temp_sensor_id);
}

/* Create FSM */
fsm_thermostat_t *fsm_thermostat_new(uint32_t led_heat_id, uint32_t led_comfort_id, uint32_t temp_sensor_id)
{
    // Do malloc for the whole FSM structure to reserve memory for the rest of the FSM, although I interpret it as fsm_t which is the first field of the structure so that the FSM library can work with it
    fsm_thermostat_t *p_fsm_thermostat = malloc(sizeof(fsm_thermostat_t));

    // Initialize the FSM
    fsm_thermostat_init(p_fsm_thermostat, led_heat_id, led_comfort_id, temp_sensor_id);

    return p_fsm_thermostat;
}

void fsm_thermostat_fire(fsm_thermostat_t *p_fsm)
{
    fsm_fire((fsm_t *)p_fsm); // Call the FSM fire function
}