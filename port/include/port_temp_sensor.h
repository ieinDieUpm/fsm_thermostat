/**
 * @file port_temp_sensor.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the temperature sensor port layer.
 * @version 0.1
 * @date 2025-05-01
 *
 */

#ifndef PORT_TEMP_SENSOR_H
#define PORT_TEMP_SENSOR_H

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdint.h>
#include <stdbool.h>

/* Defines and macros --------------------------------------------------------*/
// HW Nucleo-STM32F446RE:
#define PORT_TEMP_SENSOR_THERMOSTAT_ID 0 /*!< Thermostat temperature sensor identifier @hideinitializer */


/**
 * @brief Gets the temperature in Celsius of the temperature sensor.
 * 
 * @param temp_sensor_id Temperature sensor ID.
 *
 * @return double Temperature in Celsius.
 */
double port_temp_sensor_get_temperature(uint32_t temp_sensor_id);

/**
 * @brief Saves the ADC value of the temperature sensor and converts it to Celsius.
 *
 * @param temp_sensor_id Temperature sensor ID.
 * @param adc_value ADC value of the temperature sensor.
 */
void port_temp_sensor_save_adc_value(uint32_t temp_sensor_id, double adc_value);

/**
 * @brief Initializes the temperature sensor.
 *
 * @param temp_sensor_id Temperature sensor ID.
 */
void port_temp_sensor_init(uint32_t temp_sensor_id);

/**
 * @brief Starts the ADC conversion of the temperature sensor.
 *
 * @param temp_sensor_id Temperature sensor ID.
 */
void port_temp_sensor_start_conversion(uint32_t temp_sensor_id);

#endif /* PORT_TEMP_SENSOR_H */