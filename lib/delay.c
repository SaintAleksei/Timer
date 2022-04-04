#include <delay.h>
#include <stm32f0xx.h>

void delay(uint32_t microsec)
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_CLKSOURCE_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_TICKINT_Msk;
    SysTick->CTRL &= ~SysTick_CTRL_COUNTFLAG_Msk;
    SysTick->VAL = 0;
    SysTick->LOAD = SystemCoreClock / 8000000 * microsec;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;

    while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));

    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}
