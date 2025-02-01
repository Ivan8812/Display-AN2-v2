#pragma once

#ifdef __cplusplus
#include <CBuf.h>
namespace rs422
{
	extern CBuf<uint8_t, 10> buf_in;
	extern CBuf<uint8_t, 10> buf_out;
}
extern "C"
{
#endif

void rs422_serve(void);
void rs422_test_rx2tx(void);
void rs422_printf(const char* fmt,...);

#ifdef __cplusplus
} // extern "C"
#endif

