/**
 * @file port_temp_sensor.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Port layer for a temperature sensor.
 * @version 0.1
 * @date 2025-05-01
 *
 */

/* Standard C includes */
#include <stdint.h>
#include <stdio.h>

/* HW dependent includes */
#include "port_system.h"
#include "port_temp_sensor.h"

/* Microcontroller dependent includes */
#include "stm32f4_temp_sensor.h"

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the HW dependencies of a temperature sensor.
 */
typedef struct
{
    GPIO_TypeDef *p_port;         /*!< GPIO where the temperature is connected */
    uint8_t pin;                  /*!< Pin/line where the temperature is connected */
    ADC_TypeDef *p_adc;           /*!< ADC where the temperature is connected */
    uint32_t adc_channel;         /*!< ADC channel where the temperature is connected */
    IRQn_Type adc_irq;            /*!< ADC IRQn */
    uint8_t adc_irq_prio;         /*!< ADC IRQn priority */
    uint8_t adc_irq_subprio;      /*!< ADC IRQn subpriority */
    ADC_HandleTypeDef adc_handle; /*!< ADC handle */
    double temperature_celsius;   /*!< Temperature in Celsius */
} stm32f4_temp_hw_t;

/* Global variables -----------------------------------------------------------*/
static stm32f4_temp_hw_t temp_sensor_thermostat_arr[] = {
    [PORT_TEMP_SENSOR_THERMOSTAT_ID] = {.p_port = STM32F4_TEMP_SENSOR_THERMOSTAT_GPIO_PORT, .pin = STM32F4_TEMP_SENSOR_THERMOSTAT_GPIO_PIN, .p_adc = STM32F4_TEMP_SENSOR_THERMOSTAT_ADC, .adc_channel = STM32F4_TEMP_SENSOR_THERMOSTAT_ADC_CHANNEL, .temperature_celsius = 0, .adc_irq = STM32F4_TEMP_SENSOR_THERMOSTAT_ADC_IRQ, .adc_irq_prio = STM32F4_TEMP_SENSOR_THERMOSTAT_ADC_IRQ_PRIO, .adc_irq_subprio = STM32F4_TEMP_SENSOR_THERMOSTAT_ADC_IRQ_SUBPRIO}};

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
    uint32_t mvolts = (VDD_VALUE * adcValue) / ((1 << adc_res_bits) - 1);
    return mvolts;
}

/* Function definitions ------------------------------------------------------*/
double port_temp_sensor_get_temperature(uint32_t temp_sensor_id)
{
    // Get the temperature structure from the array
    stm32f4_temp_hw_t *p_temp = &temp_sensor_thermostat_arr[temp_sensor_id];

    // Get the temperature sensor structure from the array
    double temp_celsius = p_temp->temperature_celsius;
    return temp_celsius;
}

void port_temp_sensor_save_adc_value(uint32_t temp_sensor_id, double adc_value)
{
    // Get the temperature structure from the array
    stm32f4_temp_hw_t *p_temp = &temp_sensor_thermostat_arr[temp_sensor_id];

    // Convert the ADC value to temperature in Celsius.
    // LM35 sensor has a linear response of 10mV/°C
    p_temp->temperature_celsius = _adc_to_mvolts(adc_value, 12) / 10.0;

    // There are few problems to print double values using printf with SWO. The value is multiplied by 10 and printed as an integer the decimal point is added manually.
    printf("Temperature: %ld.%d oC\n", (uint32_t)(p_temp->temperature_celsius), (uint8_t)((10 * p_temp->temperature_celsius)) % 10);
}
void port_temp_sensor_init(uint32_t temp_sensor_id)
{
    // Get the temperature structure from the array
    stm32f4_temp_hw_t *p_temp = &temp_sensor_thermostat_arr[temp_sensor_id];

    // 1. Inicializar GPIO como entrada analógica
    GPIO_TypeDef *p_port = p_temp->p_port;
    uint16_t pin = p_temp->pin;

    GPIO_InitTypeDef temp_sensor_gpio = {0};

    if (p_port == GPIOA)
        __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (p_port == GPIOB)
        __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (p_port == GPIOC)
        __HAL_RCC_GPIOC_CLK_ENABLE();

    temp_sensor_gpio.Pin = pin;
    temp_sensor_gpio.Mode = GPIO_MODE_ANALOG;
    temp_sensor_gpio.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(p_port, &temp_sensor_gpio);

    // Get the ADC instance
    ADC_TypeDef *p_adc = p_temp->p_adc;

    // Get the ADC handler
    ADC_HandleTypeDef *adc_handle = &p_temp->adc_handle;

    // 2. Habilitar reloj ADC
    if (p_adc == ADC1)
    {
        __HAL_RCC_ADC1_CLK_ENABLE();
    }
    else if (p_adc == ADC2)
    {
        __HAL_RCC_ADC2_CLK_ENABLE();
    }
    else if (p_adc == ADC3)
    {
        __HAL_RCC_ADC3_CLK_ENABLE();
    }

    // 3. Reset del ADC (equivalente CMSIS)
    __HAL_RCC_ADC_FORCE_RESET();
    __NOP();
    __HAL_RCC_ADC_RELEASE_RESET();

#if defined(USE_ADC_TEMP_VREFINT)
    // Activar sensor de temperatura y Vrefint si se usa
    __HAL_ADC_TEMPERATURE_SENSOR_ENABLE();
#endif

    // 4. Asignar instancia y deshabilitar antes de iniciar
    adc_handle->Instance = p_adc;
    __HAL_ADC_DISABLE(adc_handle);

    // 5. Configuración HAL del ADC
    adc_handle->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
    adc_handle->Init.Resolution = ADC_RESOLUTION_12B;
    adc_handle->Init.ScanConvMode = DISABLE;
    adc_handle->Init.ContinuousConvMode = DISABLE;
    adc_handle->Init.DiscontinuousConvMode = DISABLE;
    adc_handle->Init.NbrOfDiscConversion = 0;
    adc_handle->Init.ExternalTrigConv = ADC_SOFTWARE_START;
    adc_handle->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    adc_handle->Init.DataAlign = ADC_DATAALIGN_RIGHT;
    adc_handle->Init.NbrOfConversion = 1;
    adc_handle->Init.DMAContinuousRequests = DISABLE;
    adc_handle->Init.EOCSelection = ADC_EOC_SINGLE_CONV;

    if (HAL_ADC_Init(adc_handle) != HAL_OK)
    {
        printf("ERROR: HAL_ADC_Init failed\n");
        while (1)
            ;
    }

    // 6. Configuración del canal
    ADC_ChannelConfTypeDef temp_sensor_channel = {0};
    temp_sensor_channel.Channel = p_temp->adc_channel; // Debe ser tipo ADC_CHANNEL_x
    temp_sensor_channel.Rank = 1;
    temp_sensor_channel.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    temp_sensor_channel.Offset = 0;

    if (HAL_ADC_ConfigChannel(adc_handle, &temp_sensor_channel) != HAL_OK)
    {
        printf("ERROR: HAL_ADC_ConfigChannel failed\n");
        while (1)
            ;
    }

    // 7. Habilitar interrupciones en NVIC
    HAL_NVIC_SetPriority(p_temp->adc_irq, p_temp->adc_irq_prio, p_temp->adc_irq_subprio);
    HAL_NVIC_EnableIRQ(p_temp->adc_irq);
}

void port_temp_sensor_start_conversion(uint32_t temp_sensor_id)
{
    // Get the temperature structure from the array
    stm32f4_temp_hw_t *p_temp = &temp_sensor_thermostat_arr[temp_sensor_id];

    // Start the ADC conversion
    HAL_ADC_Start_IT(&p_temp->adc_handle);
}

ADC_HandleTypeDef *stm32f4_get_temp_sensor_adc_handler(uint32_t temp_sensor_id)
{
    // Get the temperature structure from the array
    stm32f4_temp_hw_t *p_temp = &temp_sensor_thermostat_arr[temp_sensor_id];

    // Return the ADC handle
    return &p_temp->adc_handle;
}