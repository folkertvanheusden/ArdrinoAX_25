#include <Arduino.h>

#include "kiss.h"
#include "target_serial.h"


target_serial::target_serial(QueueHandle_t out, Print *const p, const int id) :
	target(out, p, id)
{
}

target_serial::~target_serial()
{
}

std::optional<target_msg_t> target_serial::wait_for_receive_packet()
{
	auto msg       = wait_for_kiss(Serial);
	auto unwrapped = unwrap_kiss(msg);

	if (unwrapped.has_value() == false)
		return { };

	return { { id, new std::vector<uint8_t>(unwrapped.value()) } };
}

void target_serial::send_message(const target_msg_t & msg)
{
	auto wrapped = wrap_kiss(*msg.data);
	Serial.write(wrapped.data(), wrapped.size());
}
