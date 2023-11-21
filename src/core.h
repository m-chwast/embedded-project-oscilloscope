#include <stdint.h>

extern uint32_t sysTicks;

inline void delay(int s)
{
	sysTicks = 0;
	while (sysTicks < s);
}
