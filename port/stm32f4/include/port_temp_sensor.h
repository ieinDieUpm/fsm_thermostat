/**
 * @file port_temp_sensor.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the temperature sensor port layer.
 * @version 0.1
 * @date 2024-05-01
 *
 */

#ifndef PORT_TEMP_SENSOR_H
#define PORT_TEMP_SENSOR_H

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdbool.h>

/* HW dependent includes */
#include "port_system.h"

/* Defines and macros --------------------------------------------------------*/
// HW Nucleo-STM32F446RE:
#define TEMP_SENSOR_THERMOSTAT_GPIO GPIOA /*!< GPIO port of the temperature sensor in the Nucleo board */
#define TEMP_SENSOR_THERMOSTAT_PIN 0      /*!< GPIO pin of the temperature sensor in the Nucleo board */
#define TEMP_SENSOR_THERMOSTAT_ADC ADC1   /*!< ADC of the temperature sensor in the Nucleo board */

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the HW dependencies of a temperature sensor.
 */
typedef struct
{
    GPIO_TypeDef *p_port;       /*!< GPIO where the temperature is connected */
    uint8_t pin;                /*!< Pin/line where the temperature is connected */
    ADC_TypeDef *p_adc;         /*!< ADC where the temperature is connected */
    double temperature_celsius; /*!< Temperature in Celsius */
} port_temp_hw_t;

/* Global variables -----------------------------------------------------------*/
extern port_temp_hw_t temp_sensor_thermostat; /*!< Temperature sensor of the thermostat system. Public for access to interrupt handlers. */

/**
 * @brief Gets the temperature in Celsius of the temperature sensor.
 *
 * @return double Temperature in Celsius.
 */
double port_temp_sensor_get_temperature(port_temp_hw_t *pir_sensor);

/**
 * @brief Saves the ADC value of the temperature sensor and converts it to Celsius.
 *
 * @param p_temp Pointer to the temperature sensor structure.
 * @param adc_value ADC value of the temperature sensor.
 */
void port_temp_sensor_save_adc_value(port_temp_hw_t* p_temp, double adc_value);

/**
 * @brief Initializes the temperature sensor.
 *
 * @param pir_sensor Pointer to the temperature sensor structure.
 */
void port_temp_sensor_init(port_temp_hw_t *pir_sensor);

#endif /* PORT_TEMP_SENSOR_H */