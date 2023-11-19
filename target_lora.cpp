#include "lora.h"
#include "target_lora.h"


target_lora::target_lora(QueueHandle_t out, Print *const p, const int id, const int pin_nss, const int pin_reset, const int pin_dio0) :
	target(out, p, id)
{
	if (init_lora(pin_nss, pin_reset, pin_dio0) == false)
		p->println(F("LoRa radio init failed"));
}

target_lora::~target_lora()
{
}

std::optional<target_msg_t> target_lora::wait_for_receive_packet()
{
	auto data = get_lora();

	if (data)
		return { { id, data } };

	return { };
}

void target_lora::send_message(const target_msg_t & msg)
{
	put_lora(*msg.data);
}
