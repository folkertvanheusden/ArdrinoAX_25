#include "lora.h"
#include "target_lora.h"


target_lora::target_lora(QueueHandle_t out, Print *const p, const int pin_nss, const int pin_reset, const int pin_dio0, const int id) :
	target(out, p, id)
{
	init_lora(pin_nss, pin_reset, pin_dio0);
}

target_lora::~target_lora()
{
}

std::optional<target_msg_t> target_lora::wait_for_receive_packet()
{
	auto data = get_lora();

	if (data.has_value())
		return { { id, std::move(data.value()) } };

	return { };
}

void target_lora::send_message(const target_msg_t & msg)
{
	put_lora(msg.data);
}
