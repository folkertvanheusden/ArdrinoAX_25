#include <LittleFS.h>
#include <stdarg.h>

#include "ax25.h"
#include "display_u8g2.h"
#include "gps.h"
#include "kiss.h"
#include "router.h"
#include "target_aprs.h"
#include "target_beacon.h"
#include "target_bluetooth.h"
#include "target_lora.h"
#include "target_serial.h"
#include "target_udp.h"
#include "wifi.h"


bool          with_wifi = true;
display      *d         = nullptr;
QueueHandle_t q         = xQueueCreate(4 /* arbitrary */, sizeof(target_msg_t));
std::vector<target *> targets;
int           target_id = 0;
gps          *g         = nullptr;

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

	pinMode(LED_BUILTIN, OUTPUT);

	d = create_lilygo_display();
	d->println("Hello!");

	esp_log_set_vprintf(espressif_log);

        if (!LittleFS.begin())
                d->println(F("Filesystem not available"));

	g = new gps(d);

	if (with_wifi && !start_wifi(*d))
		d->println(F("WiFi failed"));

	targets.push_back(new target_serial(q, d, target_id++));
	targets.push_back(new target_lora(q, d, target_id++, 18, 23, 26));
	targets.push_back(new target_beacon(q, d, target_id++, "PD9FVH", 0, "Dit is een test.", 300000l));
	if (g)
		targets.push_back(new target_aprs(q, d, target_id++, "PD9FVH", 0, g, "www.vanheusden.com", 200000l, 100, true));
	if (with_wifi)
		targets.push_back(new target_udp(q, d, target_id++, 5001, "192.168.64.206"));

	// bluetooth needs the wifi stack to be in sleep-mode
	if (with_wifi) {
		while(get_wifi_on_line() == false)  // DON'T do if no wifi
			vTaskDelay(10 / portTICK_PERIOD_MS);
	}
	targets.push_back(new target_bluetooth(q, d, target_id++, "0000", "PD9FVH"));
}

void loop() {
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

	router(q, d, &targets);
}
