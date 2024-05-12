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

/* Private functions ---------------------------------------------------------*/
void port_thermostat_timer_setup(fsm_thermostat_t *p_thermostat)
{

    if (THERMOSTAT_MEASUREMENT_TIMER == TIM2)
    {
        // Enable the timer clock
        RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    }

    // Disable the timer
    THERMOSTAT_MEASUREMENT_TIMER->CR1 &= ~TIM_CR1_CEN;

    // Autoreload preload enabled
    THERMOSTAT_MEASUREMENT_TIMER->CR1 |= TIM_CR1_ARPE;

    // Reset the values of the timer
    THERMOSTAT_MEASUREMENT_TIMER->CNT = 0;

    // Set the timeout value
    // Compute ARR and PSC to match the duration. Check if the duration is too long and adapt prescaler and ARR
    double sec = (double)p_thermostat->timer_period_sec;
    double scc = (double)SystemCoreClock;
    double psc = round(((scc * sec) / (65535.0 + 1.0)) - 1.0);
    double arr = round(((scc * sec) / (psc + 1.0)) - 1.0);

    // Adjust psc and arr if necessary
    if (arr > 0xFFFF)
    {
        psc += 1.0;
        arr = round((scc * sec) / (psc + 1.0) - 1.0);
    }

    // Load the values
    THERMOSTAT_MEASUREMENT_TIMER->ARR = (uint32_t)(round(arr));
    THERMOSTAT_MEASUREMENT_TIMER->PSC = (uint32_t)(round(psc));

    // Clean interrupt flags
    THERMOSTAT_MEASUREMENT_TIMER->SR &= ~TIM_SR_UIF;

    // Enable the update interrupt
    THERMOSTAT_MEASUREMENT_TIMER->DIER |= TIM_DIER_UIE;

    if (THERMOSTAT_MEASUREMENT_TIMER == TIM2)
    {
        // Enable the interrupt in the NVIC
        NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 2, 0));
        NVIC_EnableIRQ(TIM2_IRQn);
    }

    THERMOSTAT_MEASUREMENT_TIMER->EGR |= TIM_EGR_UG; // 6) Update generation: Re-inicializa el contador y actualiza los registros. IMPORTANTE que esté lo último


    // Enable the timer
    THERMOSTAT_MEASUREMENT_TIMER->CR1 |= TIM_CR1_CEN;
}