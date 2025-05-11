/**
 * @file stm32f4_led.c
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Port layer for LEDs connected to the STM32F4 Nucleo board.
 * @version 0.1
 * @date 2025-05-01
 *
 */

/* Standard C includes */
#include <stdbool.h>
#include <math.h>

/* HW dependent includes */
#include "port_system.h"
#include "port_led.h"

/* Microcontroller dependent includes */
#include "stm32f4_led.h"

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the HW dependencies of an RGB LED.
 *
 */
typedef struct
{
    GPIO_TypeDef *p_port; /*!< GPIO where the LED is connected */
    uint16_t pin;         /*!< Pin/line where the LED is connected */
} stm32f4_led_hw_t;

/* Global variables -----------------------------------------------------------*/
/**
 * @brief Array of LED structures to define the HW dependencies of the LEDs.
 *
 */
static stm32f4_led_hw_t leds_arr[] = {
    [PORT_LED_HEAT_ID] = {
        .p_port = STM32F4_LED_HEAT_GPIO_PORT,
        .pin = STM32F4_LED_HEAT_GPIO_PIN,
    },
    [PORT_LED_COMFORT_ID] = {
        .p_port = STM32F4_LED_COMFORT_GPIO_PORT,
        .pin = STM32F4_LED_COMFORT_GPIO_PIN,
    },
    [PORT_LED_ON_ID] = {
        .p_port = STM32F4_LED_ON_GPIO_PORT,
        .pin = STM32F4_LED_ON_GPIO_PIN,
    }
};

/* Function definitions ------------------------------------------------------*/
void port_led_init(uint32_t led_id)
{
    // Get the LED structure from the array
    stm32f4_led_hw_t *p_led = &leds_arr[led_id];

    // Get the GPIO port and pin
    GPIO_TypeDef *p_port = p_led->p_port;
    uint16_t pin = p_led->pin;

    // Initialize the GPIO port and pin
    // Create a generic LED structure to configure the GPIO
    GPIO_InitTypeDef led = {0}; // Initialize to 0 all fields by default

    /* GPIO Ports Clock Enable */
    if (p_port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (p_port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (p_port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }

    /* Configure GPIO pin */
    led.Pin = pin;
    led.Mode = MODE_OUTPUT; // A LED is an output device
    led.Pull = GPIO_NOPULL; // No pull-up or pull-down internal resistors

    /* Initialize GPIO pin */
    HAL_GPIO_Init(p_port, &led);

    /* Set GPIO pin output level to low */
    HAL_GPIO_WritePin(p_port, pin, GPIO_PIN_RESET);
}

bool port_led_get_value(uint32_t led_id)
{
    // Get the LED structure from the array
    stm32f4_led_hw_t *p_led = &leds_arr[led_id];

    // Get the port and pin
    GPIO_TypeDef *p_port = p_led->p_port;
    uint16_t pin = p_led->pin;

    // Return the LED state
    return HAL_GPIO_ReadPin(p_port, pin);
}

void port_led_set_value(uint32_t led_id, bool value)
{
    // Get the LED structure from the array
    stm32f4_led_hw_t *p_led = &leds_arr[led_id];

    // Get the port and pin
    GPIO_TypeDef *p_port = p_led->p_port;

    uint16_t pin = p_led->pin;

    // Set the LED state
    HAL_GPIO_WritePin(p_port, pin, value);
}

void port_led_toggle(uint32_t led_id)
{
    // Get the LED structure from the array
    stm32f4_led_hw_t *p_led = &leds_arr[led_id];

    // Get the port and pin
    GPIO_TypeDef *p_port = p_led->p_port;
    uint16_t pin = p_led->pin;

    // Toggle the LED state
    HAL_GPIO_TogglePin(p_port, pin);
}