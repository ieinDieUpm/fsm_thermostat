/**
 * @file port_led.h
 * @author Josué Pagán (j.pagan@upm.es)
 * @brief Header file for the LED port layer.
 * @date 01-01-2024
 */
#ifndef PORT_LED_H_
#define PORT_LED_H_

/* Includes ------------------------------------------------------------------*/
/* Standard C includes */
#include <stdbool.h>

/* HW dependent includes */
#include "port_system.h"

/* Defines and macros --------------------------------------------------------*/
// HW Nucleo-STM32F446RE:
#define LED_HEAT_GPIO GPIOA /*!< GPIO port of the heating LED */
#define LED_HEAT_PIN 5      /*!< GPIO pin of the heating LED */
#define LED_COOL_GPIO GPIOB /*!< GPIO port of the deactivation LED */
#define LED_COOL_PIN 4      /*!< GPIO pin of the deactivation LED */

/* Typedefs --------------------------------------------------------------------*/
/**
 * @brief Structure to define the HW dependencies of a LED.
 */
typedef struct
{
    GPIO_TypeDef *p_port; /*!< GPIO where the LED is connected */
    uint8_t pin;          /*!< Pin/line where the LED is connected */
} port_led_hw_t;

/* Global variables -----------------------------------------------------------*/
extern port_led_hw_t led_heater_active; /*!< Heating LED of the thermostat. */
extern port_led_hw_t led_comfort_temperature; /*!< Cooling LED of the thermostat. */

/* Function prototypes and explanations ---------------------------------------*/
/**
 * @brief Initializes the LED.
 *
 * @param p_led Pointer to the LED structure.
 */
void port_led_init(port_led_hw_t *p_led);

/**
 * @brief Returns the current state of the LED.
 *
 * @return true if the LED is on
 * @return false if the LED is off
 */
bool port_led_get_status(port_led_hw_t *p_led);

/**
 * @brief Turn on the LED
 *
 */
void port_led_on(port_led_hw_t *p_led);

/**
 * @brief Turn off the LED
 *
 */
void port_led_off(port_led_hw_t *p_led);

/**
 * @brief Toggles the LED state.
 *
 */
void port_led_toggle(port_led_hw_t *p_led);

#endif // PORT_LED_H_
