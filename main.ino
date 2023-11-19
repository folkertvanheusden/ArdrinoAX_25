#include <display_u8g2.h>
#include <stdarg.h>

#include "wifi.h"


display *d = nullptr;

int espressif_log(const char *fmt, va_list args) {
	int len = vsnprintf(nullptr, 0, fmt, args);
	char *buffer = (char *)malloc(len + 1);
	vsprintf(buffer, fmt, args);
Serial.println(buffer);  // TODO
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
}

void loop() {
}
