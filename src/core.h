#ifndef __CORE_HEADER__
#define __CORE_HEADER__

#include <stdint.h>

#include "uart.h"

#if defined(_DEBUG)
#define ASSERT(text) UART_Send(text)
#else
#define ASSERT(text)
#endif

extern uint32_t sysTicks;

#define delay(s) sysTicks = 0; while(sysTicks < (s))

#define swapInt(l, r) { int temp = (l); (l) = (r); (r) = temp; }

#endif // __CORE_HEADER__
