#include "../../../Drivers/CMSIS/Device/ST/STM32F1xx/Include/stm32f103xb.h"

/* Глобальные переменные */
volatile uint8_t button_pressed = 0;      /* Флаг нажатия кнопки */
volatile uint8_t button_debounce = 0;     /* Флаг защиты от дребезга */

/**
 * @brief  Задержка
 */
void Delay(volatile uint32_t count) {

    while (count--) { __NOP(); }
}

/**
 * @brief  Инициализация GPIO
 * @note   Кнопка подключена к GND (PA0 → кнопка → GND)
 *         Внутренний pull-up резистор включён
 */
void GPIO_Init(void) {

    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    /* PB2 - выход для светодиода */
    GPIOB->CRL &= ~GPIO_CRL_MODE2;
    GPIOB->CRL |= GPIO_CRL_MODE2_1;      /* 50 МГц */
    GPIOB->CRL &= ~GPIO_CRL_CNF2;        /* Push-pull */
    GPIOB->BSRR = GPIO_BSRR_BR2;         /* LED выключен */

    /* PA0 - вход с pull-up (кнопка на GND) */
    GPIOA->CRL &= ~GPIO_CRL_MODE0;       /* Вход */
    GPIOA->CRL &= ~GPIO_CRL_CNF0;        /* Очистка CNF */
    GPIOA->CRL |= GPIO_CRL_CNF0_1;       /* CNF[1:0] = 10 - pull-up/pull-down */
    GPIOA->BSRR = GPIO_BSRR_BS0;         /* Включить pull-up */
}

/**
 * @brief  Инициализация EXTI для кнопки на PA0
 * @note   Прерывание по падающему фронту (нажатие = переход 1→0)
 */
void EXTI_Init(void) {
    
    RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;     /* Тактирование AFIO */

    AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0; /* EXTI0 = PA0 */

    EXTI->FTSR |= EXTI_FTSR_FT0;            /* Падающий фронт */
    EXTI->RTSR &= ~EXTI_RTSR_RT0;           /* Запрет восходящего */

    EXTI->IMR |= EXTI_IMR_MR0;              /* Разрешение прерывания */

    NVIC_EnableIRQ(EXTI0_IRQn);             /* Включение в NVIC */
    NVIC_SetPriority(EXTI0_IRQn, 1);        /* Приоритет */
}

/**
 * @brief  Чтение кнопки
 * @retval 1 - кнопка нажата (pull-up)
 * @retval 0 - кнопка отпущена (gnd)
 */
uint8_t Button_Read(void)
{
    return (GPIOA->IDR & GPIO_IDR_IDR0) ? 1 : 0;
}

/**
 * @brief  Чтение кнопки с антидребезгом
 */
uint8_t Button_Read_Debounced(void)
{
    static uint8_t last_state = 0;
    uint8_t current = Button_Read();
    
    if (current != last_state) {
        Delay(20000);  /* ~20 мс */
        current = Button_Read();
        last_state = current;
    }
    return current;
}

void LED_On(void)  { GPIOB->BSRR = GPIO_BSRR_BS2; }
void LED_Off(void) { GPIOB->BSRR = GPIO_BSRR_BR2; }
void LED_Toggle(void) { GPIOB->ODR ^= GPIO_ODR_ODR2; }

int main(void) {
    volatile uint32_t delay = 300000;
    uint8_t btn;

    GPIO_Init();
    EXTI_Init();

    __enable_irq();  /* Разрешение глобальных прерываний */
    Delay(100000);

    while (1)
    {
        /* ===== ТЕСТ 1: Мигание ===== */
        #if 0
        LED_On();
        Delay(delay);
        LED_Off();
        Delay(delay);
        #endif
        /* ===== ТЕСТ 2: LED горит при нажатой кнопке (опрос) ===== */
        /* Раскомментировать для проверки */
        #if 0
        btn = Button_Read();
        if (btn != 0) {      // 0 = нажата (GND)
            LED_On();
        } else {
            LED_Off();
        }
        #endif

        /* ===== ТЕСТ 3: Переключение по кнопке (опрос с антидребезгом) ===== */
        /* Раскомментировать для проверки */
        #if 0 
        if (Button_Read_Debounced() == 0) {  // Нажатие
            LED_Toggle();
            Delay(200000);  // Задержка после нажатия
        }
        #endif

        /* ===== ТЕСТ 4: Обработка через флаг из прерывания ===== */
        /* Раскомментировать для проверки прерывания */
        #if 1
        if (button_pressed) {
            button_pressed = 0;
            button_debounce = 0;
            Delay(250);  // Антидребезг
            if (Button_Read() == 0) {  // Проверка что кнопка ещё нажата
                LED_Toggle();
            }
        }
        #endif
    }
}
