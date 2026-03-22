/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f1xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "stm32f1xx_it.h"

/* USER CODE BEGIN EV */
extern volatile uint8_t button_pressed;
extern volatile uint8_t button_debounce;
/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
void NMI_Handler(void) { while (1) {} }
void HardFault_Handler(void) { while (1) {} }
void MemManage_Handler(void) { while (1) {} }
void BusFault_Handler(void) { while (1) {} }
void UsageFault_Handler(void) { while (1) {} }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
//void SysTick_Handler(void) {}

/******************************************************************************/
/* STM32F1xx Peripheral Interrupt Handlers                                    */
/******************************************************************************/

/* USER CODE BEGIN 1 */
/**
  * @brief  Обработчик прерывания EXTI0 (кнопка на PA0)
  * @note   Срабатывает по падающему фронту (нажатие кнопки: 1→0)
  */
void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0)  /* Проверка флага линии 0 */
    {
        /* Защита от дребезга - первое срабатывание */
        if (!button_debounce)
        {
            button_debounce = 1;
            button_pressed = 1;
        }
        
        EXTI->PR = EXTI_PR_PR0;  /* Очистка флага */
    }
}
/* USER CODE END 1 */
