#include "console.h"
#include "uart.h"


static void ProcessMsg(const char* msg)
{
	
}

void Console_Task(void)
{
	bool uartMsgReady = UART_IsMsgReady();
	if(uartMsgReady)
		{
			char tmpMsg[UART_BUFFER_SIZE];
			UART_GetMessage(tmpMsg);
			UART_Send(tmpMsg);	//send received msg on console
			ProcessMsg(tmpMsg);
		}
}
