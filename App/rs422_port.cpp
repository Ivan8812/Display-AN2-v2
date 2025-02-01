#include <rs422_port.h>
#include <cstdarg>
#include <cstdio>
#include <CBuf.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvolatile"
#include "usart.h"
#include "stm32h7xx_hal_uart.h"
#pragma GCC diagnostic pop

namespace rs422
{
	CBuf<uint8_t, 10> buf_in;
	CBuf<uint8_t, 10> buf_out;
}
static __UART_HandleTypeDef* const uart = &huart4;


using namespace std;
using namespace rs422;


//------------------------------------------------------------------------------
void rs422_serve()
{
	while(__HAL_UART_GET_FLAG(uart, UART_FLAG_RXFNE))
	{
		if(buf_in.full())
			buf_in.pop();
		buf_in.push(uart->Instance->RDR & 0xFF);
	}

	while(__HAL_UART_GET_FLAG(uart, UART_FLAG_TXFNF) && !buf_out.empty())
		uart->Instance->TDR = (uint32_t)buf_out.pop();
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void rs422_test_rx2tx(void)
{
	while(!buf_in.empty() && !buf_out.full())
		buf_out.push(buf_in.pop());
}
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
void rs422_printf(const char* fmt,...)
{
	static char str[128];
	va_list args;
	va_start(args, fmt);
	vsprintf(str, fmt, args);
	va_end(args);

	char* p = str;
	while(*p)
		buf_out.push(*p++);
}
//------------------------------------------------------------------------------

