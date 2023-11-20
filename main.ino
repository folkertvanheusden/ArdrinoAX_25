#include <ArduinoJson.h>
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
#include "version.h"
#include "wifi.h"

constexpr char cfg_file[] = "/configuration.json";
display      *d         = nullptr;
QueueHandle_t q         = xQueueCreate(4 /* arbitrary */, sizeof(target_msg_t));
std::vector<target *> targets;
int           target_id = 0;
gps          *g         = nullptr;

struct SpiRamAllocator {
	void *allocate(size_t size) {
		return heap_caps_malloc(size, MALLOC_CAP_SPIRAM);
	}

	void deallocate(void *pointer) {
		heap_caps_free(pointer);
	}

	void* reallocate(void *ptr, size_t new_size) {
		return heap_caps_realloc(ptr, new_size, MALLOC_CAP_SPIRAM);
	}
};

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
	d->println(version_str);

	esp_log_set_vprintf(espressif_log);

	BasicJsonDocument<SpiRamAllocator> doc(4096);
        if (LittleFS.begin()) {
		File cfg_file_h = LittleFS.open(cfg_file, "r");
		if (cfg_file_h) {
			deserializeJson(doc, cfg_file_h);
			cfg_file_h.close();
		}
		else {
			d->println(F("No configuration file!"));
		}
	}
	else {
                d->println(F("Filesystem not available"));
	}

	if (doc["with-gps"])
		g = new gps(d);

	bool with_wifi = doc[F("with-wifi")];

	if (with_wifi && !start_wifi(*d))
		d->println(F("WiFi failed"));

	if (doc[F("with-serial-tnc")])
		targets.push_back(new target_serial(q, d, target_id++));

	if (doc[F("with-lora")]) {
		int pin_nss   = doc[F("lora-pin-nss")];
		int pin_reset = doc[F("lora-pin-reset")];
		int pin_dio0  = doc[F("lora-pin-dio0")];
		targets.push_back(new target_lora(q, d, target_id++, pin_nss, pin_reset, pin_dio0));
	}

	int callsign_ssid = doc[F("callsign-ssid")];
	if (callsign_ssid > 15)
		callsign_ssid -= '0';

	if (doc[F("with-beacon")]) {
		unsigned long beacon_interval_s = doc[F("beacon-interval-s")];

		targets.push_back(new target_beacon(q, d, target_id++, doc[F("callsign")], callsign_ssid, doc[F("beacon-text")], beacon_interval_s * 1000l));
	}

	if (g && doc[F("with-aprs")]) {
		std::optional<double> aprs_distance_interval;

		if (doc.containsKey(F("aprs-distance-interval")))
			aprs_distance_interval = doc[F("aprs-distance-interval")];

		unsigned long max_aprs_beacon_interval_s = doc[F("max-aprs-beacon-interval-s")];

		targets.push_back(new target_aprs(q, d, target_id++, doc[F("callsign")], callsign_ssid, g, doc[F("aprs-beacon-text")], max_aprs_beacon_interval_s * 1000l, aprs_distance_interval, doc[F("with-aprs-oe-format")]));
	}

	if (with_wifi && doc[F("with-ipd")])
		targets.push_back(new target_udp(q, d, target_id++, doc[F("ipd-udp-port")], doc[F("ipd-udp-target-address")]));

	if (doc[F("with-bluetooth")]) {
		// bluetooth needs the wifi stack to be in sleep-mode
		if (with_wifi) {
			while(get_wifi_on_line() == false)  // DON'T do if no wifi
				vTaskDelay(10 / portTICK_PERIOD_MS);
		}

		targets.push_back(new target_bluetooth(q, d, target_id++, doc[F("bluetooth-pin")], doc["callsign"]));
	}
}

void loop() {
	digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

	router(q, d, &targets);
}
