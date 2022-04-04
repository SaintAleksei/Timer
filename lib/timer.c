#include <stm32f0xx.h>
#include <fdss.h>

#define TIME_MAX (60 * 99 + 59)
#define TIME_PUSH (50)

static uint32_t time;
static uint8_t run = 0;
static uint32_t step = 1;

void timer_Init()
{
    FDSS_Init();
    SystemCoreClockUpdate();

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
    SysTick->LOAD = SystemCoreClock / 8000;
    SysTick->VAL  = 0;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;

    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER4, 0);
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER5, 0);
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER6, 0);
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODER7, 0);

    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    SYSCFG->EXTICR[2] = SYSCFG_EXTICR2_EXTI4_PB |
                        SYSCFG_EXTICR2_EXTI5_PB |
                        SYSCFG_EXTICR2_EXTI6_PB |
                        SYSCFG_EXTICR2_EXTI7_PB;

    EXTI->IMR |= EXTI_IMR_MR4 | 
                 EXTI_IMR_MR5 |
                 EXTI_IMR_MR6 |
                 EXTI_IMR_MR7; 

    EXTI->RTSR |= EXTI_RTSR_TR4 |
                  EXTI_RTSR_TR5 |
                  EXTI_RTSR_TR6 |
                  EXTI_RTSR_TR7;

    EXTI->FTSR |= EXTI_FTSR_TR4 |
                  EXTI_FTSR_TR5 |
                  EXTI_FTSR_TR6 |
                  EXTI_FTSR_TR7;

    NVIC_EnableIRQ(EXTI4_15_IRQn);
    NVIC_SetPriority(EXTI4_15_IRQn, 1);
}

void SysTick_Handler()
{
    static uint8_t pos;
    static uint32_t counter;

    if (!run)
        counter = 0;
    else
        counter++;

    if (time && counter == 1000)
    {
        counter = 0;
        time--;
    }
    
    FDSS_SetPos(pos);
    
    switch (pos)
    {
        case 0:
            FDSS_SetDigit((time % 60) % 10);
            break;
        case 1:
            FDSS_SetDigit((time % 60) / 10);
            break;
        case 2:
            FDSS_SetDigitDot((time / 60) % 10);
            break;
        case 3:
            FDSS_SetDigit((time / 60) / 10);
            break;
        default:
            pos = 0;
    }

    pos++;
    pos %= 4;
}

#define BUTTON_TICKS 100

void EXTI4_15_IRQHandler()
{
    static uint8_t start_pb4;
    static uint8_t start_pb5;
    static uint8_t start_pb6;
    static uint8_t start_pb7;
    
    int counter_pb4 = 0;
    int counter_pb5 = 0;
    int counter_pb6 = 0;
    int counter_pb7 = 0;

    for (int i = 0; i < BUTTON_TICKS; i++)
    {
        uint32_t mask = GPIOB->IDR;

        if (mask & GPIO_IDR_4)
            counter_pb4++;
        else
            counter_pb4--;

        if (mask & GPIO_IDR_5)
            counter_pb5++;
        else
            counter_pb5--;

        if (mask & GPIO_IDR_6)
            counter_pb6++;
        else
            counter_pb6--;

        if (mask & GPIO_IDR_7)
            counter_pb7++;
        else
            counter_pb7--;
    }

    if (counter_pb7 >= 0 && start_pb7)
    {
        start_pb7 = 0;
        if (run)
            run = 0;
        else 
            run = 1;
    }
    if (counter_pb7 < 0 && !start_pb7)
        start_pb7 = 1;

    if (!run && counter_pb6 > 0 && start_pb6)
    {
        start_pb6 = 0;
        if (time + step <= TIME_MAX)
            time += step;
    }

    if (!run && counter_pb6 < 0 && !start_pb6)
        start_pb6 = 1;

    if (!run && counter_pb5 > 0 && start_pb5)
    {
        start_pb5 = 0;
        if (time >= step)
            time -= step;
    }

    if (!run && counter_pb5 < 0 && !start_pb5)
        start_pb5 = 1;

    if (!run && counter_pb4 > 0 && start_pb4)
    {
        start_pb4 = 0;
        if (step == 1)
            step = 10;
        else if (step == 10)
            step = 60;
        else if (step == 60)
            step = 600;
        else if (step == 600)
            step = 1;
    }

    if (!run && counter_pb4 < 0 && !start_pb4)
        start_pb4 = 1;

    EXTI->PR &= ~EXTI_PR_PR7;
    EXTI->PR &= ~EXTI_PR_PR6;
    EXTI->PR &= ~EXTI_PR_PR5;
    EXTI->PR &= ~EXTI_PR_PR4;
}
