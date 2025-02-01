#pragma once

#include <cstdint>
#include <functional>

class RotaryEncoder
{
public:
	using enc_handler_t = std::function<void(int8_t delta)>;
	using btn_handler_t = std::function<void()>;
	using tim_handler_t = std::function<void()>;
	using listener_t = struct
	{
		enc_handler_t on_rotate;
		btn_handler_t on_click;
		btn_handler_t on_hold;
		tim_handler_t on_timeout;
	};
	//---------------------------------------------------------------------------
	RotaryEncoder(listener_t& listener): listener(listener) {}
	//---------------------------------------------------------------------------
	void change_listener(listener_t& new_listener) { listener = new_listener;}
	//---------------------------------------------------------------------------
	void serve_input(int8_t enc, bool btn)
	{
		if(btn)
		{
			if(hold_cntr <= HoldThr)
				hold_cntr++;
			if(hold_cntr == HoldThr)
			{
				btn_handler_t on_hold = listener.on_hold;
				on_hold();
			}
			timeout_cntr = 0;
		}
		else
		{
			if((hold_cntr > 10) && (hold_cntr < 400))
			{
				btn_handler_t on_click = listener.on_click;
				on_click();
				timeout_cntr = 0;
			}
			else if(enc != enc_prev)
			{
				enc_handler_t on_rotate = listener.on_rotate;
				on_rotate(enc-enc_prev);
				timeout_cntr = 0;
			}
			else
			{
				if(timeout_cntr <= Timeout)
					timeout_cntr++;
				if(timeout_cntr == Timeout)
				{
					btn_handler_t on_timeout = listener.on_timeout;
					on_timeout();
				}
			}
			hold_cntr = 0;
		}
		enc_prev = enc;
	}
	//---------------------------------------------------------------------------
private:
	static const uint32_t HoldThr = 1000;
	static const uint32_t Timeout = 5000;
	uint32_t timeout_cntr = 0;
	uint32_t hold_cntr = 0;
	listener_t listener;
	int8_t enc_prev = 0;
};
