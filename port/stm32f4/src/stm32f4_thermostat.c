/**
 * @file port_thermostat.c
 * @file port_thermostat.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Source file for the thermostat system port layer.
 * @version 0.1
 * @date 2024-05-01
 *
 */

/* Standard C includes */
#include <stdlib.h>
#include <math.h>

/* Project includes */
#include "port_thermostat.h"

/* Microcontroller dependent includes */
#include "stm32f4_thermostat.h"

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the HW dependencies of a thermostat.
 *
 */
typedef struct
{
    TIM_TypeDef *p_timer;                     /*!< Timer to control the LED */
    IRQn_Type timer_irq;                      /*!< Timer IRQn */
    uint8_t timer_irq_prio;                   /*!< Timer IRQn priority */
    uint8_t timer_irq_subprio;                /*!< Timer IRQn subpriority */
    uint32_t timer_period;                    /*!< Timer period in milliseconds */
    TIM_HandleTypeDef handler_tim_thermostat; /*!< Timer handler */
} stm32f4_thermostat_hw_t;

/* Global variables -----------------------------------------------------------*/
/**
 * @brief Array of thermostat structures to define the HW dependencies of the thermostats.
 *
 */
static stm32f4_thermostat_hw_t thermostats_arr[] = {
    [PORT_THERMOSTAT_ID] = {
        .p_timer = STM32F4_THERMOSTAT_MEASUREMENT_TIMER,
        .timer_irq = STM32F4_THERMOSTAT_MEASUREMENT_TIMER_IRQ,
        .timer_irq_prio = STM32F4_THERMOSTAT_MEASUREMENT_TIMER_IRQ_PRIO,
        .timer_irq_subprio = STM32F4_THERMOSTAT_MEASUREMENT_TIMER_IRQ_SUBPRIO,
        .timer_period = STM32F4_THERMOSTAT_MEASUREMENT_TIMER_PERIOD_MS,
    },
};

/* Private functions ---------------------------------------------------------*/
void port_thermostat_timer_setup(uint32_t thermostat_id)
{
    // Get the thermostat structure from the array
    stm32f4_thermostat_hw_t *p_thermostat = &thermostats_arr[thermostat_id];

    // Get the timer instance
    TIM_TypeDef *p_timer = p_thermostat->p_timer;

    // Get the timer handler
    TIM_HandleTypeDef *p_handler_timer = &p_thermostat->handler_tim_thermostat;

    // Enable the timer clock using HAL macro
    if (p_timer == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    else if (p_timer == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    else
    {
        // Invalid timer
        return;
    }

    // Assign the timer instance to the handler
    p_handler_timer->Instance = p_timer;

    // Stop timer with interrupt enabled
    HAL_TIM_Base_Stop_IT(p_handler_timer);

    // Compute ARR and PSC to match the duration in milliseconds. Check if the duration is too long and adapt prescaler and ARR
    double ms = (double)p_thermostat->timer_period;
    double scc = HSI_VALUE; // HSI clock frequency in Hz
    double psc = round((((scc / 1000.0) * ms) / (65535.0 + 1.0)) - 1.0);
    double arr = round((((scc / 1000.0) * ms) / (psc + 1.0)) - 1.0);

    // Adjust psc and arr if necessary
    while (arr > 0xFFFF)
    {
        psc += 1.0;
        arr = round(((scc / 1000.0) * ms) / (psc + 1.0) - 1.0);
    }

    // Set the timer parameters
    // Set the prescaler and auto-reload register
    // Update timer initialization structure with computed PSC and ARR values
    p_handler_timer->Init.Prescaler = (uint32_t)(round(psc));
    p_handler_timer->Init.Period = (uint32_t)(round(arr));

    // Enable the auto-reload preload
    p_handler_timer->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

    // Initialize the timer with the new PSC and ARR values
    HAL_TIM_Base_Init(p_handler_timer);

    // Reset counter value
    __HAL_TIM_SET_COUNTER(p_handler_timer, 0);

    // limpiar flag de update (MUY IMPORTANTE para evitar interrupción falsa)
    __HAL_TIM_CLEAR_FLAG(p_handler_timer, TIM_FLAG_UPDATE);

    // Set priority and enable IRQ in NVIC
    HAL_NVIC_SetPriority(p_thermostat->timer_irq, p_thermostat->timer_irq_prio, p_thermostat->timer_irq_subprio);

    // Enable the timer
    HAL_TIM_Base_Start_IT(p_handler_timer);

    // Enable the timer IRQ
    HAL_NVIC_EnableIRQ(p_thermostat->timer_irq);
}

TIM_HandleTypeDef *stm32f4_get_thermostat_timer_handler(uint32_t thermostats_id)
{
    // Get the thermostat structure from the array
    stm32f4_thermostat_hw_t *p_thermostat = &thermostats_arr[thermostats_id];

    // Return the timer handler
    return &p_thermostat->handler_tim_thermostat;
}