#ifndef LIB_TIMER
#define LIB_TIMER

#include <stdint.h>

void timer_Init();
void timer_Setup(uint32_t time);
void timer_Start();
void timer_Stop();

#endif
