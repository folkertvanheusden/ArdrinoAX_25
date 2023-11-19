#include <stdarg.h>

#include "ax25.h"
#include "display_u8g2.h"
#include "target_lora.h"
#include "target_serial.h"
#include "target_udp.h"
#include "wifi.h"


display      *d         = nullptr;
QueueHandle_t q         = xQueueCreate(4 /* arbitrary */, sizeof(target_msg_t));
std::vector<target *> targets;
int           target_id = 0;

int espressif_log(const char *fmt, va_list args) {
	int   len    = vsnprintf(nullptr, 0, fmt, args);

	char *buffer = (char *)malloc(len + 1);
	vsprintf(buffer, fmt, args);
	d->print(buffer);
	free(buffer);

	return len;
}

void setup() {
	Serial.begin(115200);

	d = create_lilygo_display();
	d->println("Hello!");

	esp_log_set_vprintf(espressif_log);

	if (!start_wifi(*d))
		d->println(F("WiFi failed"));

	targets.push_back(new target_serial(q, d, target_id++));
	targets.push_back(new target_lora(q, d, target_id++, 18, 23, 26));
	targets.push_back(new target_udp(q, d, target_id++, 5001, "192.168.64.206"));
}

void loop() {
	target_msg_t msg;
	if (xQueueReceive(q, &msg, portMAX_DELAY) == pdTRUE) {
		ax25_packet a25(*msg.data);
		bool is_valid = a25.get_valid();
		d->printf("%s -> %s (%d)", a25.get_from().to_str().c_str(), a25.get_to().to_str().c_str(), is_valid);

		if (is_valid) {
			for(auto & t: targets)
				t->queue_message(msg);
		}

		delete msg.data;
	}
}
