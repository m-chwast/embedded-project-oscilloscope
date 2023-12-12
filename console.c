#include "core.h"

#include <string.h>
#include <stdio.h>
#include "console.h"
#include "uart.h"


static bool ProcessCommandsNoParam(const char* msg)
{
	bool processed = true;
	char cmd[UART_BUFFER_SIZE];
	
	strcpy(cmd, msg);
	*strstr(cmd, "\r\n") = '\0';
	
	if(strcmp(cmd, "start") == 0)
	{
		
	}
	else if(strcmp(cmd, "single") == 0)
	{
		
	}
	else if(strcmp(cmd, "stop") == 0)
	{
		
	}
	else
	{
		processed = false;
	}
	return processed;
}

static bool ProcessCommandsWithParam(const char* msg)
{
	return false;
}


static void ProcessMsg(const char* msg)
{
	if(msg[0] == '\0' || strcmp(msg, "\r\n") == 0)
	{
		return;
	}
	
	
	bool processed = ProcessCommandsNoParam(msg);
	if(processed == false)
		processed = ProcessCommandsWithParam(msg);
	
	if(processed)
		UART_Send("Cmd processed\r\n");
	else
		UART_Send("Cmd unknown\r\n");
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
