#include <fdss.h>
#include <stm32f0xx.h>
#include <delay.h>

/*
     6
   5   1
     7 
   4   2
     3   8
*/

#define S1 1U << 7
#define S2 1U << 3
#define S3 1U << 1
#define S4 1U << 0
#define S5 1U << 6
#define S6 1U << 5
#define S7 1U << 4
#define S8 1U << 2

#define D1 1U << 0
#define D2 1U << 1
#define D3 1U << 2
#define D4 1u << 3

#define DIGIT_MASK (S1 | S2 | S3 | S4 | S5 | S6 | S7 | S8)

/*
     6
   5   1
     7 
   4   2
     3   8
*/

static const uint8_t digit[10] =
{
    S1 | S2 | S3 | S4 | S5 | S6,
    S1 | S2,
    S6 | S1 | S7 | S4 | S3,
    S6 | S1 | S7 | S2 | S3,
    S5 | S7 | S1 | S2,
    S6 | S5 | S7 | S2 | S3,
    S6 | S5 | S4 | S3 | S2 | S7,
    S6 | S1 | S2,
    S1 | S2 | S3 | S4 | S5 | S6 | S7,
    S1 | S2 | S3 | S5 | S6 | S7,
};

#define POS_MASK (D1 | D2 | D3 | D4)

static const uint8_t pos[4] =
{
    D2 | D3 | D4,
    D1 | D3 | D4,
    D1 | D2 | D4,
    D1 | D2 | D3,
};

void FDSS_Init()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOCEN;

    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER0, GPIO_MODER_MODER0_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER1, GPIO_MODER_MODER1_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER2, GPIO_MODER_MODER2_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER3, GPIO_MODER_MODER3_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER4, GPIO_MODER_MODER4_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER5, GPIO_MODER_MODER5_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER6, GPIO_MODER_MODER6_0);
    MODIFY_REG(GPIOA->MODER, GPIO_MODER_MODER7, GPIO_MODER_MODER7_0);

    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER0, GPIO_MODER_MODER0_0);
    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER1, GPIO_MODER_MODER1_0);
    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER2, GPIO_MODER_MODER2_0);
    MODIFY_REG(GPIOC->MODER, GPIO_MODER_MODER3, GPIO_MODER_MODER3_0);
}

void FDSS_SetDigit(uint8_t num)
{
    GPIOA->ODR &= ~DIGIT_MASK;
    GPIOA->ODR |= digit[num % 10];
}

void FDSS_SetDigitDot(uint8_t num)
{
    GPIOA->ODR &= ~DIGIT_MASK;
    GPIOA->ODR |= digit[num % 10] | S8;
}

void FDSS_SetPos(uint8_t position)
{
    GPIOC->ODR &= ~POS_MASK;
    GPIOC->ODR |= pos[position % 4];
}

void FDSS_Test()
{
    FDSS_Init();

    for (uint8_t i = 0; i < 4; i++)
    {
        FDSS_SetPos(i);
        for (uint8_t i = 0; i < 10; i++)
        {
            FDSS_SetDigit(i);
            delay(500000);
            FDSS_SetDigitDot(i);
            delay(500000);
        }
    }
}
