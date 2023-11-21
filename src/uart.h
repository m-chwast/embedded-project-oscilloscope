#ifndef UART_H
#define UART_H

void UART_Init(void);
void UART_Send(const char* txt);
void UART_SendNum(int num);

#endif
