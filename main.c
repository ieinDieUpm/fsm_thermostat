/**
 * @file main.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Basic FSM reading analog data from a linear thermistor LM35
 * @version 0.1
 * @date 2024-05-01
 *
 */

/* INCLUDES */
#include <stdio.h>
#include "port_system.h"
#include "fsm_thermostat.h"

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
    fsm_t *p_fsm_thermostat = fsm_thermostat_new(&led_heater_active, &led_comfort_temperature, &temp_sensor_thermostat);

    while (1)
    {
        // Launch the FSM
        fsm_fire(p_fsm_thermostat);

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