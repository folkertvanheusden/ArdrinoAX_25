#include "gps.h"
#include "str.h"


#if defined(T_BEAM_V1_2) || defined(T_BEAM_V1_0)
constexpr uint8_t pin_rx = 12;
constexpr uint8_t pin_tx = 34;
#else
constexpr uint8_t pin_rx = 255;
constexpr uint8_t pin_tx = 255;
#endif

HardwareSerial SerialGPS(1);

void gps_task(void *par)
{
	gps *g = reinterpret_cast<gps *>(par);

	for(;;)
		g->process_nmea();
}

gps::gps(Print *const p): p(p)
{
	if (pin_rx == 255) {
		p->println(F("GPS disabled"));
		return;
	}

	SerialGPS.begin(9600, SERIAL_8N1, pin_tx, pin_rx);

	BaseType_t xReturned = xTaskCreate(
                    gps_task,        /* Function that implements the task. */
                    "GPS",           /* Text name for the task. */
                    2048,            /* Stack size in words, not bytes. */
                    (void *)this,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &task);          /* Used to pass out the created task's handle. */

	if (xReturned != pdPASS)
		p->println(F("Failed to create \"GPS\" task"));
}

gps::~gps()
{
}

std::optional<std::pair<double, double> > gps::get_position()
{
	std::optional<std::pair<double, double> > out;

	if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
		if (is_valid)
			out = { lat, lng };

		xSemaphoreGive(semaphore);
	}

	return out;
}

std::optional<double> gps::get_speed()
{
	std::optional<double> out;

	if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
		if (is_valid)
			out = spd;

		xSemaphoreGive(semaphore);
	}

	return out;
}

void gps::process_nmea()
{
	for(;;) {
		if (SerialGPS.available() == 0) {
			vTaskDelay(10 / portTICK_PERIOD_MS);
			continue;
		}

		if (gps_processor.encode(SerialGPS.read())) {
			if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
				if (gps_processor.location.isValid()) {
					if (gps_processor.location.isUpdated()) {
						lat = gps_processor.location.lat();
						lng = gps_processor.location.lng();
					}

					if (gps_processor.speed.isUpdated())
						spd = gps_processor.speed.kmph();

					if (!is_valid) {
						is_valid = true;
						p->println(F("Have GPS fix"));
					}
				}
				else if (is_valid) {
					is_valid = false;
					p->println(F("No GPS fix"));
				}

				xSemaphoreGive(semaphore);
			}
		}
	}
}

std::string gps_double_to_aprs(double lat, double lng) {
        double lata = abs(lat);
        double latd = floor(lata);
        double latm = (lata - latd) * 60;
        double lath = (latm - floor(latm)) * 100;
        double lnga = abs(lng);
        double lngd = floor(lnga);
        double lngm = (lnga - lngd) * 60;
        double lngh = (lngm - floor(lngm)) * 100;

	char buffer[19];

        snprintf(buffer, sizeof buffer, "%02d%02d.%02d%c/%03d%02d.%02d%c",
                        int(latd), int(floor(latm)), int(floor(lath)), lat > 0 ? 'N' : 'S',
                        int(lngd), int(floor(lngm)), int(floor(lngh)), lng > 0 ? 'E' : 'W');

	return buffer;
}
