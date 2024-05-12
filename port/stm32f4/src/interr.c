/**
 * @file interr.c
 * @brief Interrupt service routines for the STM32F4 platform.
 * @author Josué Pagán (j.pagan@upm.es)
 * @date 2024-04-01
 */
// Include headers of different port elements:
#include "stm32f4xx.h"
#include "port_system.h"
#include "port_led.h"
#include "port_temp_sensor.h"

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
 *
 */
void TIM2_IRQHandler(void)
{
  // Start the ADC conversion
  port_system_adc_start_conversion(temp_sensor_thermostat.p_adc, temp_sensor_thermostat.pin);
  
  TIM2->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag
}

/**
 * @brief Interrupt service routine for all the ADCs.
 *
 * @note This ISR is called when any ADC generates an interrupt.
 *
 */
void ADC_IRQHandler(void)
{
  // Identify if the ADC that generated the interrupt is the same as the temperature sensor
  if (temp_sensor_thermostat.p_adc->SR & ADC_SR_EOC)
  {
    // Read the temperature sensor value
    port_temp_sensor_save_adc_value(&temp_sensor_thermostat, temp_sensor_thermostat.p_adc->DR);
    
    // Clear the ADC interrupt flag
    temp_sensor_thermostat.p_adc->SR &= ~ADC_SR_EOC;
  }
}