/**
 * @file port_system.c
 * @brief File that defines the functions that are related to the access to the specific HW of the microcontroller.
 * @author Sistemas Digitales II
 * @date 2024-01-01
 */

/* Includes ------------------------------------------------------------------*/
#include "port_system.h"

/* Defines -------------------------------------------------------------------*/
#define HSI_VALUE ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz */

/* GLOBAL VARIABLES */
static volatile uint32_t msTicks = 0; /*!< Variable to store millisecond ticks. @warning **It must be declared volatile!** Just because it is modified in an ISR. **Add it to the definition** after *static*. */

/* These variables are declared extern in CMSIS (system_stm32f4xx.h) */
uint32_t SystemCoreClock = HSI_VALUE;                                               /*!< Frequency of the System clock */
const uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9}; /*!< Prescaler values for AHB bus */
const uint8_t APBPrescTable[8] = {0, 0, 0, 0, 1, 2, 3, 4};                          /*!< Prescaler values for APB bus */

//------------------------------------------------------
// SYSTEM CONFIGURATION
//------------------------------------------------------
/**
 * @brief  Setup the microcontroller system
 *         Initialize the FPU setting, vector table location and External memory
 *         configuration.
 *
 * @note   This function is called at startup by CMSIS in startup_stm32f446xx.s.
 */
void SystemInit(void)
{
/* FPU settings ------------------------------------------------------------*/
#if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
  SCB->CPACR |= ((3UL << 10 * 2) | (3UL << 11 * 2)); /* set CP10 and CP11 Full Access */
#endif

#if defined(DATA_IN_ExtSRAM) || defined(DATA_IN_ExtSDRAM)
  SystemInit_ExtMemCtl();
#endif /* DATA_IN_ExtSRAM || DATA_IN_ExtSDRAM */

  /* Configure the Vector Table location -------------------------------------*/
#if defined(USER_VECT_TAB_ADDRESS)
  SCB->VTOR = VECT_TAB_BASE_ADDRESS | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
#endif                                                 /* USER_VECT_TAB_ADDRESS */
}

/**
 * @brief System Clock Configuration
 *
 * @attention This function should NOT be accesible from the outside to avoid configuration problems.
 * @note This function starts a system timer that generates a SysTick every 1 ms.
 * @retval None
 */
void system_clock_config(void)
{
  /** Configure the main internal regulator output voltage */
  /* Power controller (PWR) */
  /* Control the main internal voltage regulator output voltage to achieve a trade-off between performance and power consumption when the device does not operate at the maximum frequency */
  PWR->CR &= ~PWR_CR_VOS; // Clean and set value
  PWR->CR |= (PWR_CR_VOS & (POWER_REGULATOR_VOLTAGE_SCALE3 << PWR_CR_VOS_Pos));

  /* Initializes the RCC Oscillators. */
  /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
  RCC->CR &= ~RCC_CR_HSITRIM; // Clean and set value
  RCC->CR |= (RCC_CR_HSITRIM & (RCC_HSI_CALIBRATION_DEFAULT << RCC_CR_HSITRIM_Pos));

  /* RCC Clock Config */
  /* Initializes the CPU, AHB and APB buses clocks */
  /* To correctly read data from FLASH memory, the number of wait states (LATENCY)
      must be correctly programmed according to the frequency of the CPU clock
      (HCLK) and the supply voltage of the device. */

  /* Increasing the number of wait states because of higher CPU frequency */
  FLASH->ACR = FLASH_ACR_LATENCY_2WS; /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */

  /* Change in clock source is performed in 16 clock cycles after writing to CFGR */
  RCC->CFGR &= ~RCC_CFGR_SW; // Clean and set value
  RCC->CFGR |= (RCC_CFGR_SW & (RCC_CFGR_SW_HSI << RCC_CFGR_SW_Pos));

  /* Update the SystemCoreClock global variable */
  SystemCoreClock = HSI_VALUE >> AHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE) >> RCC_CFGR_HPRE_Pos];

  /* Configure the source of time base considering new system clocks settings */
  SysTick_Config(SystemCoreClock / (1000U / TICK_FREQ_1KHZ)); /* Set Systick to 1 ms */
}

size_t port_system_init()
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  /* Configure Flash prefetch, Instruction cache, Data cache */
  /* Instruction cache enable */
  FLASH->ACR |= FLASH_ACR_ICEN;

  /* Data cache enable */
  FLASH->ACR |= FLASH_ACR_DCEN;

  /* Prefetch cache enable */
  FLASH->ACR |= FLASH_ACR_PRFTEN;

  /* Set Interrupt Group Priority */
  NVIC_SetPriorityGrouping(NVIC_PRIORITY_GROUP_4);

  /* Use systick as time base source and configure 1ms tick (default clock after Reset is HSI) */
  /* Configure the SysTick IRQ priority. It must be the highest (lower number: 0)*/
  NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 0U, 0U)); /* Tick interrupt priority */

  /* Init the low level hardware */
  /* Reset and clock control (RCC) */
  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; /* Syscfg clock enabling */

  /* Peripheral clock enable register */
  RCC->APB1ENR |= RCC_APB1ENR_PWREN; /* PWREN: Power interface clock enable */

  /* Configure the system clock */
  system_clock_config();

  return 0;
}

//------------------------------------------------------
// TIMER RELATED FUNCTIONS
//------------------------------------------------------
uint32_t port_system_get_millis()
{
  return msTicks;
}

void port_system_set_millis(uint32_t ms)
{
  msTicks = ms;
}

void port_system_delay_ms(uint32_t ms)
{
  uint32_t tickstart = port_system_get_millis();

  while ((port_system_get_millis() - tickstart) < ms)
  {
  }
}

void port_system_delay_until_ms(uint32_t *p_t, uint32_t ms)
{
  uint32_t until = *p_t + ms;
  uint32_t now = port_system_get_millis();
  if (until > now)
  {
    port_system_delay_ms(until - now);
  }
  *p_t = port_system_get_millis();
}

//------------------------------------------------------
// GPIO RELATED FUNCTIONS
//------------------------------------------------------
void port_system_gpio_config(GPIO_TypeDef *p_port, uint8_t pin, uint8_t mode, uint8_t pupd)
{
  if (p_port == GPIOA)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; /* GPIOA_CLK_ENABLE */
  }
  else if (p_port == GPIOB)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN; /* GPIOB_CLK_ENABLE */
  }
  else if (p_port == GPIOC)
  {
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; /* GPIOC_CLK_ENABLE */
  }

  /* Clean ( &=~ ) by displacing the base register and set the configuration ( |= ) */
  p_port->MODER &= ~(GPIO_MODER_MODER0 << (pin * 2U));
  p_port->MODER |= (mode << (pin * 2U));

  p_port->PUPDR &= ~(GPIO_PUPDR_PUPD0 << (pin * 2U));
  p_port->PUPDR |= (pupd << (pin * 2U));
}

void port_system_gpio_config_exti(GPIO_TypeDef *p_port, uint8_t pin, uint32_t mode)
{
  uint32_t port_selector = 0;

  RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

  /* SYSCFG external interrupt configuration register */
  if (p_port == GPIOA)
  {
    port_selector = 0;
  }
  else if (p_port == GPIOB)
  {
    port_selector = 1;
  }
  else if (p_port == GPIOC)
  {
    port_selector = 2;
  }

  uint32_t base_mask = 0x0FU;
  uint32_t displacement = (pin % 4) * 4;

  SYSCFG->EXTICR[pin / 4] &= ~(base_mask << displacement);
  SYSCFG->EXTICR[pin / 4] |= (port_selector << displacement);

  /* Rising trigger selection register (EXTI_RTSR) */
  EXTI->RTSR &= ~BIT_POS_TO_MASK(pin);
  if (mode & TRIGGER_RISING_EDGE)
  {
    EXTI->RTSR |= BIT_POS_TO_MASK(pin);
  }

  /* Falling trigger selection register (EXTI_FTSR) */
  EXTI->FTSR &= ~BIT_POS_TO_MASK(pin);
  if (mode & TRIGGER_FALLING_EDGE)
  {
    EXTI->FTSR |= BIT_POS_TO_MASK(pin);
  }

  /* Event mask register (EXTI_EMR) */
  EXTI->EMR &= ~BIT_POS_TO_MASK(pin);
  if (mode & TRIGGER_ENABLE_EVENT_REQ)
  {
    EXTI->EMR |= BIT_POS_TO_MASK(pin);
  }

  /* Clear EXTI line configuration */
  EXTI->IMR &= ~BIT_POS_TO_MASK(pin);

  /* Interrupt mask register (EXTI_IMR) */
  if (mode & TRIGGER_ENABLE_INTERR_REQ)
  {
    EXTI->IMR |= BIT_POS_TO_MASK(pin);
  }
}

void port_system_gpio_exti_enable(uint8_t pin, uint8_t priority, uint8_t subpriority)
{
  NVIC_SetPriority(GET_PIN_IRQN(pin), NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, subpriority));
  NVIC_EnableIRQ(GET_PIN_IRQN(pin));
}

void port_system_gpio_exti_disable(uint8_t pin)
{
  NVIC_DisableIRQ(GET_PIN_IRQN(pin));
}

void port_system_gpio_config_alternate(GPIO_TypeDef *p_port, uint8_t pin, uint8_t alternate)
{
  uint32_t base_mask = 0x0FU;
  uint32_t displacement = (pin % 8) * 4;

  p_port->AFR[(uint8_t)(pin / 8)] &= ~(base_mask << displacement);
  p_port->AFR[(uint8_t)(pin / 8)] |= (alternate << displacement);
}

//----------------------processing--------------------------------
// ADC RELATED FUNCTIONS
//------------------------------------------------------
void port_system_adc_single_ch_init(ADC_TypeDef *p_adc, uint8_t channel, uint32_t cr_mode)
{
  // First of all, enable the source clock of the ADC before configuring any of its registers
  if (p_adc == ADC1)
  {
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; /* ADC1_CLK_ENABLE */
  }
  else if (p_adc == ADC2)
  {
    RCC->APB2ENR |= RCC_APB2ENR_ADC2EN; /* ADC2_CLK_ENABLE */
  }
  else if (p_adc == ADC3)
  {
    RCC->APB2ENR |= RCC_APB2ENR_ADC3EN; /* ADC3_CLK_ENABLE */
  }

  //-------------------------------------------------------------------------------------------
  // 	ADC common configuration (ADC common control register (ADC_CCR))
  //-------------------------------------------------------------------------------------------
  // Reset the ADC (common to all ADCs)
  RCC->APB2RSTR |= RCC_APB2RSTR_ADCRST;

  // Short delay
  __NOP(); // Also can be used 	(void)RCC->AHB2RSTR; as an example

  // Clear the reset of the ADC (common to all ADCs)
  RCC->APB2RSTR &= ~RCC_APB2RSTR_ADCRST;

#if defined(USE_ADC_TEMP_VREFINT)
  // Temperature sensor and Vref internal channels enable
  ADC123_COMMON->CCR |= ADC_CCR_TSVREFE;
#endif

  // ADC prescaler to select the frequency of the clock to the ADC. By default, input ADC clock PCLK2 divided by 2.
  // The clock of the ADC (ADCCLK) is common to all the ADCs. The clock is generated from the APB2 clock divided by a programmable prescaler (ADCPRE) that allows the ADC to work at a frequencies of fPCLK2/2, fPCLK2/4, fPCLK2/6, or fPCLK2/8.
  ADC123_COMMON->CCR &= ~ADC_CCR_ADCPRE;

  // DMA configuration (DMA mode). 0: DMA mode disabled by default.
  ADC123_COMMON->CCR &= ~ADC_CCR_DMA;
  
  // Delay between 2 sampling phases. By default, 0000: 5*T_ADCCLK. This is used in dual and triple interleaved modes.
  ADC123_COMMON->CCR &= ~ADC_CCR_DELAY;

  // Multi ADC mode selection. By default, 00000; All ADCs independent.
  ADC123_COMMON->CCR &= ~ADC_CCR_MULTI;

  //-------------------------------------------------------------------------------------------
  // 	ADC particular configuration (ADCx control register (ADC_CR1 and ADC_CR2))
  //-------------------------------------------------------------------------------------------
  // Disable the ADC
  p_adc->CR2 &= ~ADC_CR2_ADON;

  // ADC continuous conversion mode. 0: Single conversion mode by default.
  p_adc->CR2 &= ~ADC_CR2_CONT;

  // ADC DMA mode selection. 0: DMA mode disabled by default.
  p_adc->CR2 &= ~ADC_CR2_DMA;

  // ADC End of conversion selection in single mode. 1: End of conversion by EOC bit by default.
  p_adc->CR2 |= ADC_CR2_EOCS;

  // ADC alignment. 0: Right alignment by default.
  p_adc->CR2 &= ~ADC_CR2_ALIGN;

  // ADC configuration register 1 (ADC_CR1)
  p_adc->CR1 = 0;

  // Set some configuration bits of the ADC_CR1 register that are currently available to be set in this PORT implementation
  // Interrupt enable (EOC, EOS, AWD, JEOC, JEOS)
  p_adc->CR1 |= (cr_mode & ADC_CR1_EOCIE_Msk);

  // Resolution of the ADC. 00: 12-bit resolution by default.
  p_adc->CR1 |= (cr_mode & ADC_CR1_RES_Msk);

  // Sampling time selection for the regular channels. 000: 3 cycles by default, for simplicity.
  channel &= 0x1FU; // Only 16 channels are available

  if (channel < 10)
  {
    p_adc->SMPR2 &= ~(ADC_SMPR2_SMP0 << (channel * 3));
  }
  else
  {
    p_adc->SMPR1 &= ~(ADC_SMPR1_SMP10 << ((channel - 10) * 3));
  }

  // Indicate how many channels are going to be converted in the sequence. 1 conversion by default.
  p_adc->SQR1 &= ~ADC_SQR1_L;
}

void port_system_adc_interrupt_enable(uint8_t priority, uint8_t subpriority)
{
  NVIC_SetPriority(ADC_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), priority, subpriority));
  NVIC_EnableIRQ(ADC_IRQn);
}

void port_system_adc_enable(ADC_TypeDef *p_adc)
{
  // Enable the ADC
  p_adc->CR2 |= ADC_CR2_ADON;

  // Wait for the ADC to be ready (approximately 10 us)
  uint32_t delay = 10000;
  while (delay--)
    ;
}

void port_system_adc_disable(ADC_TypeDef *p_adc)
{
  // Disable the ADC
  p_adc->CR2 &= ~ADC_CR2_ADON;
}

void port_system_adc_start_conversion(ADC_TypeDef *p_adc, uint8_t channel)
{
  channel &= 0x1FU; // Only 16 channels are available

  // Since we are going to convert only one channel, we are going to use the first sequence register
  p_adc->SQR3 = 0;
  p_adc->SQR3 = channel;

  // Clear the status register
  p_adc->SR = 0;

  // Start the conversion
  p_adc->CR2 |= ADC_CR2_SWSTART;
}

// ------------------------------------------------------
// POWER RELATED FUNCTIONS
// ------------------------------------------------------