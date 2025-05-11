/**
 * @file interr.c
 * @brief Interrupt service routines for the STM32F4 platform.
 * @author Josué Pagán (j.pagan@upm.es)
 * @date 2025-05-01
 */
// C Standard includes
#include <stdio.h>

// Include HW dependencies:
#include "port_system.h"

// Include headers of different port elements:
#include "port_temp_sensor.h"
#include "stm32f4_temp_sensor.h"
#include "port_thermostat.h"
#include "stm32f4_thermostat.h"

//------------------------------------------------------
// INTERRUPT SERVICE ROUTINES
//------------------------------------------------------
/**
 * @brief Interrupt service routine for the System tick timer (SysTick).
 *
 * @note This ISR is called when the SysTick timer generates an interrupt.
 * The program flow jumps to this ISR and increments the tick counter by one millisecond.
 *
 * @warning **The variable `msTicks` must be declared volatile!** Just because it is modified by a call of an ISR, in order to avoid [*race conditions*](https://en.wikipedia.org/wiki/Race_condition)
. **Added to the definition** after *static*.
 *
 */
void SysTick_Handler(void)
{
  port_system_set_millis(port_system_get_millis() + 1);
}

/**
 * @brief Interrupt service routine for the TIM2 timer.
 *
 * @note This ISR is called when the TIM2 timer generates an interrupt.
 * The program flow jumps to this ISR then this calls the generic HAL_TIM_IRQHandler() function to clear the update flag.
 *
 */
void TIM2_IRQHandler(void)
{
  // Get the timer handle of the thermostat
  TIM_HandleTypeDef *p_handler_tim_thermostat = stm32f4_get_thermostat_timer_handler(PORT_THERMOSTAT_ID);

  // Call the HAL TIM IRQ handler to clear the update interrupt flag and jump to the callback function
  HAL_TIM_IRQHandler(p_handler_tim_thermostat);
}

/**
 * @brief Interrupt service routine for all the ADCs.
 *
 * @note This ISR is called when any ADC generates an interrupt.
 *
 */
void ADC_IRQHandler(void)
{
  // Get the ADC handle
  ADC_HandleTypeDef *p_handler_adc = stm32f4_get_temp_sensor_adc_handler(PORT_TEMP_SENSOR_THERMOSTAT_ID);

  // Call the HAL ADC IRQ handler to clear the interrupt flag and handle the conversion with the callback function
  HAL_ADC_IRQHandler(p_handler_adc);
}

/**
 * @brief  Timer period elapsed callback in non blocking mode
 * @param  handler_tim: TIM handle
 *
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *handler_tim)
{
  if (handler_tim->Instance == STM32F4_THERMOSTAT_MEASUREMENT_TIMER) // If the timer is the thermostat measurement timer
  {
    // Get the ADC handle of the temperature sensor
    ADC_HandleTypeDef *p_handler_adc = stm32f4_get_temp_sensor_adc_handler(PORT_TEMP_SENSOR_THERMOSTAT_ID);

    // Start the ADC conversion
    HAL_ADC_Start_IT(p_handler_adc); // empieza una conversión con interrupción
  }
  else
  {
    // Handle other timers if needed
  }
}

/**
 * @brief ADC conversion complete callback.
 *
 * @param p_handler_adc Pointer to the ADC handle.
 *
 * @note This function is called when the ADC conversion is complete.
 */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *p_handler_adc)
{
  if (p_handler_adc->Instance == STM32F4_TEMP_SENSOR_THERMOSTAT_ADC)
  {
    uint32_t adc = HAL_ADC_GetValue(p_handler_adc);
    printf("ADC value = %lu\n", adc);

    // Call the function to save the ADC value
    port_temp_sensor_save_adc_value(PORT_TEMP_SENSOR_THERMOSTAT_ID, adc);
  }
}