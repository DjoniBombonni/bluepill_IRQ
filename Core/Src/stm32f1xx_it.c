#include "main.h"
#include "stm32f1xx_it.h"

extern volatile uint8_t button_pressed;
extern volatile uint8_t button_debounce;
extern volatile uint32_t tim_cnt;

void NMI_Handler(void) { while (1) {} }
void HardFault_Handler(void) { while (1) {} }
void MemManage_Handler(void) { while (1) {} }
void BusFault_Handler(void) { while (1) {} }
void UsageFault_Handler(void) { while (1) {} }
void SVC_Handler(void) {}
void DebugMon_Handler(void) {}
void PendSV_Handler(void) {}
void SysTick_Handler(void) {
    #if 1 
    if(tim_cnt != 0)
    tim_cnt--;
    #endif
}

void EXTI0_IRQHandler(void)
{
    if (EXTI->PR & EXTI_PR_PR0)  // Проверка флага линии 0
    {
        // Защита от дребезга - первое срабатывание
        if (!button_debounce)
        {
            button_debounce = 1;
            button_pressed = 1;
        }
        
        EXTI->PR = EXTI_PR_PR0;  // Очистка флага
    }
}
