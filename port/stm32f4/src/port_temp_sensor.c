/**
 * @file port_temp_sensor.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Port layer for a temperature sensor.
 * @version 0.1
 * @date 2024-05-01
 *
 */

/* Standard C includes */
#include <stdint.h>
#include <stdio.h>

/* HW dependent includes */
#include "port_temp_sensor.h"
#include "stm32f4xx.h"
#include "port_system.h"

/* Global variables -----------------------------------------------------------*/
port_temp_hw_t temp_sensor_thermostat = {.p_port = TEMP_SENSOR_THERMOSTAT_GPIO, .pin = TEMP_SENSOR_THERMOSTAT_PIN, .p_adc = TEMP_SENSOR_THERMOSTAT_ADC, .temperature_celsius = 1.0};

/* Private functions */

/**
 * @brief Converts an ADC value to millivolts. 
 * 
 * @param adcValue Counts of the ADC  
 * @param adc_res_bits Resolution of the ADC in bits 
 * @return uint32_t Millivolts 
 */
uint32_t _adc_to_mvolts(uint32_t adcValue, uint8_t adc_res_bits)
{
    uint32_t mvolts = (ADC_VREF_MV * adcValue) / ((1 << adc_res_bits) - 1);
    return mvolts;
}
/* Function definitions ------------------------------------------------------*/
double port_temp_sensor_get_temperature(port_temp_hw_t *p_temp)
{
    return p_temp->temperature_celsius;
}

void port_temp_sensor_save_adc_value(port_temp_hw_t *p_temp, double adc_value)
{
    // Convert the ADC value to temperature in Celsius.
    // LM35 sensor has a linear response of 10mV/°C
    p_temp->temperature_celsius = _adc_to_mvolts(adc_value, 12) / 10.0;

    // There are few problems to print double values using printf with SWO. The value is multiplied by 10 and printed as an integer the decimal point is added manually.
    printf("Temperature: %ld.%d oC\n", (uint32_t)(p_temp->temperature_celsius), (uint8_t)((10*p_temp->temperature_celsius))%10);
}

void port_temp_sensor_init(port_temp_hw_t *p_temp)
{
    // Initialize the GPIO
    port_system_gpio_config(p_temp->p_port, p_temp->pin, GPIO_MODE_ANALOG, GPIO_PUPDR_NOPULL);

    // Initialize the ADC with 12-bit resolution and EOC interrupt enable
    port_system_adc_single_ch_init(p_temp->p_adc, p_temp->pin, ADC_RESOLUTION_12B | ADC_EOC_INTERRUPT_ENABLE);

    // Enable the ADC global interrupt
    port_system_adc_interrupt_enable(1, 0);

    // Enable the ADC
    port_system_adc_enable(p_temp->p_adc);
}