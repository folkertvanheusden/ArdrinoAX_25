#include "ax25.h"
#include "kiss.h"
#include "router.h"
#include "target.h"


void router(const QueueHandle_t q, Print *const p, std::vector<target *> *const targets)
{
	target_msg_t msg;
	if (xQueueReceive(q, &msg, 200 / portTICK_PERIOD_MS) == pdTRUE) {
		ax25_packet *a25 = nullptr;

		auto unwrapped = unwrap_kiss(*msg.data);
		if (unwrapped.has_value())
			a25 = new ax25_packet(unwrapped.value());
		else
			a25 = new ax25_packet(*msg.data);

		bool is_valid = a25->get_valid();
		p->printf("%s -> %s (%d for %d - %s)", a25->get_from().to_str().c_str(), a25->get_to().to_str().c_str(), is_valid, unwrapped.has_value(), a25->get_invalid_reason().c_str());
		delete a25;

		if (is_valid) {
			for(auto & t: *targets)
				t->queue_message(msg);
		}
		else {
			std::string dummy;

			for(auto & v: *msg.data)
				dummy += myformat("%02x ", v);

			p->printf("Invalid packet: %s", dummy.c_str());
		}

		delete msg.data;
	}
}
