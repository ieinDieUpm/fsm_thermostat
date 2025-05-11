/**
 * @file main.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Basic FSM reading analog data from a linear thermistor LM35
 * @version 1.0
 * @date 2025-05-01
 *
 */

/* INCLUDES */
#include <stdio.h>
#include "port_system.h"
#include "port_led.h"
#include "fsm_thermostat.h"

/* Defines and macros --------------------------------------------------------*/

/* MAIN FUNCTION */

/**
 * @brief Main function
 *
 * @return int
 */
int main()
{
    // Local variables
    uint8_t previous_thermostat_status = UNKNOWN;

    /* Init board */
    port_system_init();

    // Create an thermostat FSM and get a pointer to it
    fsm_thermostat_t *p_fsm_thermostat = fsm_thermostat_new(PORT_LED_HEAT_ID, PORT_LED_COMFORT_ID, PORT_TEMP_SENSOR_THERMOSTAT_ID);

    while (1)
    {
        // Launch the FSM
        fsm_thermostat_fire(p_fsm_thermostat);

        uint8_t current_thermostat_status = fsm_thermostat_get_status(p_fsm_thermostat);
        if (current_thermostat_status != previous_thermostat_status)
        {
            uint32_t last_time_activated = fsm_thermostat_get_last_time_event(p_fsm_thermostat, current_thermostat_status);
            if (current_thermostat_status == ACTIVATION)
            {
                printf("Thermostat ON at %ld\n", last_time_activated);
            }
            else if (current_thermostat_status == DEACTIVATION)
            {
                printf("Thermostat OFF at %ld\n", last_time_activated);
            }
            previous_thermostat_status = current_thermostat_status;
        }
    }
    return 0;
}