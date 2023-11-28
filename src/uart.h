#ifndef UART_H
#define UART_H

#include <stdbool.h>

#define UART_BUFFER_SIZE 256


extern char UART_buffer[UART_BUFFER_SIZE];


void UART_Init(void);

void UART_Send(const char* txt);
void UART_SendNum(int num);

bool UART_ReadTask(void);
bool UART_IsMsgReady(void);
bool UART_GetMessage(char * buff);

#endif
