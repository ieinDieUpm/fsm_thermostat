/**
 * @file stm32f4_thermostat.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the thermostat system port layer for the STM32F4 platform.
 * @version 0.1
 * @date 2025-05-01
 *
 */

#ifndef STM32F4_THERMOSTAT_H
#define STM32F4_THERMOSTAT_H

/* Includes ------------------------------------------------------------------*/
/* HW dependent includes */
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

/* Defines and macros --------------------------------------------------------*/
// HW Nucleo-STM32F446RE:
#define STM32F4_THERMOSTAT_MEASUREMENT_TIMER TIM2 /*!< Timer to measure the temperature */
#define STM32F4_THERMOSTAT_MEASUREMENT_TIMER_IRQ TIM2_IRQn /*!< Timer IRQn */
#define STM32F4_THERMOSTAT_MEASUREMENT_TIMER_IRQ_PRIO 0 /*!< Timer IRQn priority */
#define STM32F4_THERMOSTAT_MEASUREMENT_TIMER_IRQ_SUBPRIO 0 /*!< Timer IRQn subpriority */
#define STM32F4_THERMOSTAT_MEASUREMENT_TIMER_PERIOD_MS 1000 /*!< Timer period in milliseconds */


/**
 * @brief Get the timer handler of the thermostat. 
 * 
 * @param thermostats_id Thermostat ID. 
 * @return TIM_HandleTypeDef* Pointer to the timer handler of the thermostat. 
 */
TIM_HandleTypeDef *stm32f4_get_thermostat_timer_handler(uint32_t thermostats_id);


#endif /* STM32F4_THERMOSTAT_H */
