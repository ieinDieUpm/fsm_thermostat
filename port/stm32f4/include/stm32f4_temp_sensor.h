/**
 * @file port_temp_sensor.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the temperature sensor port layer.
 * @version 0.1
 * @date 2025-05-01
 *
 */

#ifndef STM32F4_TEMP_SENSOR_H
#define STM32F4_TEMP_SENSOR_H

/* Includes ------------------------------------------------------------------*/
/* HW dependent includes */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"

/* Defines and macros --------------------------------------------------------*/
// HW Nucleo-STM32F446RE:
#define STM32F4_TEMP_SENSOR_THERMOSTAT_GPIO_PORT GPIOA           /*!< GPIO port of the temperature sensor in the Nucleo board */
#define STM32F4_TEMP_SENSOR_THERMOSTAT_GPIO_PIN GPIO_PIN_0       /*!< GPIO pin of the temperature sensor in the Nucleo board */
#define STM32F4_TEMP_SENSOR_THERMOSTAT_ADC ADC1                  /*!< ADC of the temperature sensor in the Nucleo board */
#define STM32F4_TEMP_SENSOR_THERMOSTAT_ADC_CHANNEL ADC_CHANNEL_0 /*!< ADC channel of the temperature sensor in the Nucleo board */
#define STM32F4_TEMP_SENSOR_THERMOSTAT_ADC_IRQ ADC_IRQn          /*!< ADC IRQn */
#define STM32F4_TEMP_SENSOR_THERMOSTAT_ADC_IRQ_PRIO 1            /*!< ADC IRQn priority */
#define STM32F4_TEMP_SENSOR_THERMOSTAT_ADC_IRQ_SUBPRIO 0         /*!< ADC IRQn subpriority */

/**
 * @brief Get the ADC handler of the temperature sensor.
 *
 * @param temp_sensor_id Temperature sensor ID.
 * @return ADC_HandleTypeDef* Pointer to the ADC handler of the temperature sensor.
 */
ADC_HandleTypeDef *stm32f4_get_temp_sensor_adc_handler(uint32_t temp_sensor_id);

#endif /* STM32F4_TEMP_SENSOR_H */