#include "../../../Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103xb.h"

#define TEST1 0
#define TEST2 0
    #if TEST2 
    uint8_t btn;
    #endif 
#define TEST3 1
#define TEST4 0

volatile uint8_t button_pressed = 0;      // флаг нажатия кнопк
volatile uint8_t button_debounce = 0;     // флаг защиты от дребезга
volatile uint32_t tim_cnt = 0;

void my_delay(uint32_t my_tick) {
    tim_cnt = my_tick;
    while(tim_cnt != 0) {
    }
}

#if 0
void Delay(volatile uint32_t count) {

    while (count--) { __NOP(); }
}
#endif

// инициалезация системного таймера
void SysTick_Init(void) {
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; 
    SysTick->LOAD = (8000 - 1);
    SysTick->VAL = (8000 - 1);
}
void GPIO_Init(void) {

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    // PB2 - выход для светодиода
    GPIOB->CRL &= ~GPIO_CRL_MODE2;
    GPIOB->CRL |= GPIO_CRL_MODE2;      // 50 МГц
    GPIOB->CRL &= ~GPIO_CRL_CNF2;        
    GPIOB->BSRR = GPIO_BSRR_BR2;         

    // PA0 - вход с pull-up
    GPIOA->CRL &= ~GPIO_CRL_MODE0;       
    GPIOA->CRL &= ~GPIO_CRL_CNF0;        
    GPIOA->CRL |= GPIO_CRL_CNF0_1;       
    GPIOA->BSRR = GPIO_BSRR_BS0;         
}

void EXTI_Init(void) {

    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     

    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0; 

    EXTI->FTSR |= EXTI_FTSR_FT0;            
    EXTI->RTSR &= ~EXTI_RTSR_RT0;           

    EXTI->IMR |= EXTI_IMR_MR0;              //Разрешение прерывания

    NVIC_EnableIRQ(EXTI0_IRQn);             // включение в NVIC
    NVIC_SetPriority(EXTI0_IRQn, 1);        
}

uint8_t Button_Read(void) { return (GPIOA->IDR & GPIO_IDR_IDR0) ? 1 : 0; }

uint8_t Button_Read_Debounced(void) {

    static uint8_t last_state = 0;
    uint8_t current = Button_Read();
    
    if (current != last_state) {
        my_delay(20000);                       // задержка ~20 мс
        current = Button_Read();
        last_state = current;
    }
    return current;
}

void LED_On(void) { GPIOB->BSRR = GPIO_BSRR_BS2; }   
void LED_Off(void) { GPIOB->BSRR = GPIO_BSRR_BR2; }
void LED_Toggle(void) { GPIOB->ODR ^= GPIO_ODR_ODR2; }

int main(void) {

    SysTick_Init();
    GPIO_Init();
    EXTI_Init();

    __enable_irq();
    my_delay(100);

    while (1) {
        
        /*=== ТЕСТ 1: Мигание PA2 ===*/ 
        //TODO: проверка работоспособности теста 1 - ОК
        #if TEST1 
        LED_Toggle();
        my_delay(300);
        #endif

        /*=== ТЕСТ 2: LED горит при нажатой кнопке ===*/ 
        //TODO: проверка работоспособности теста 2 - ОК
        #if TEST2
        btn = Button_Read();
        if (btn != 0) {
            LED_On();
        } else {
            LED_Off();
        }
        #endif

        /*=== ТЕСТ 3: Переключение по кнопке с антидребезгом ===*/ 
        //TODO проверка работоспособности теста 3 (после нажатия на кнопку светодиод мигает) - нужно исправить НЕ ОК
        #if TEST3
        if (Button_Read_Debounced() != 0) {
            LED_Toggle();
            my_delay(400);  // Задержка после нажатия
        }
        #endif

        /*=== ТЕСТ 4: Обработка через флаг из прерывания ===*/ 
        //TODO проверка работоспособности теста 4 - ОК
        #if TEST4
        if (button_pressed) {
            button_pressed = 0;
            my_delay(250);  // Антидребезг (~25 мс) TODO: расчитать точное время антидребезга
            if (Button_Read() == 0) { 
                LED_Toggle();
            }
            button_debounce = 0;  // Сброс антидребезга для следующего нажатия
        }
        #endif
    }
}
