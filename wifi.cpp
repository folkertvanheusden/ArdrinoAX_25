#include <configure_wifi.h>
#include <LittleFS.h>
#include <optional>
#include <Print.h>
#include <start_wifi.h>


static bool on_line_state { false };
static SemaphoreHandle_t on_line_semaphore = xSemaphoreCreateMutex();

static bool progress_indicator(const int nr, const int mx, const std::string & which) {
	return true;
}

static void wifi_task(void *s) {
	Print *const p = reinterpret_cast<Print *>(s);

	set_hostname("ArdrinoAX.25");

	enable_wifi_debug(p);

	for(;;) {
		scan_access_points_start();

		configure_wifi cw;

		if (cw.is_configured() == false) {
			p->println(F("Configure WiFi"));

			start_wifi("ArdrinoAX.25");  // enable wifi with AP (empty string for no AP)

			cw.configure_aps();
		}
		else {
			start_wifi({ });
		}

		// see what we can see
		p->println(F("Scanning WiFi"));
		scan_access_points_start();

		while(scan_access_points_wait() == false)
			vTaskDelay(100 / portTICK_PERIOD_MS);

		auto available_access_points = scan_access_points_get();

		// try to connect
		p->println(F("Connect WiFi"));
		auto state = try_connect_init(cw.get_targets(), available_access_points, 300, progress_indicator);
		connect_status_t cs = CS_IDLE;

		p->println(F("Waiting..."));
		unsigned long end_ts = millis() + 60000l;  // 60s wait
		while(millis() < end_ts) {
			cs = try_connect_tick(state);

			if (cs != CS_IDLE)
				break;

			p->print(F("."));

			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		while(check_wifi_connection_status() == CS_CONNECTED) {
			if (xSemaphoreTake(on_line_semaphore, portMAX_DELAY) == pdTRUE) {
				if (on_line_state == false) {
					p->println(F("WiFi connected"));
					on_line_state = true;
					WiFi.setSleep(true);
				}

				xSemaphoreGive(on_line_semaphore);
			}

			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		if (xSemaphoreTake(on_line_semaphore, portMAX_DELAY) == pdTRUE) {
			if (on_line_state == true) {
				p->println(F("WiFi disconnected"));
				on_line_state = false;
			}

			xSemaphoreGive(on_line_semaphore);
		}
	}
}

bool start_wifi(Print & p) {
	TaskHandle_t xHandle   = nullptr;
	BaseType_t   xReturned = xTaskCreate(
                    wifi_task,       /* Function that implements the task. */
                    "wifi",          /* Text name for the task. */
                    8192,            /* Stack size in words, not bytes. */
                    (void *)&p,      /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle);       /* Used to pass out the created task's handle. */

	if (xReturned == pdPASS)
		return true;

	p.println(F("Failed to create wifi task"));

	return false;
}

bool get_wifi_on_line() {
	bool rc = false;

	if (xSemaphoreTake(on_line_semaphore, portMAX_DELAY) == pdTRUE) {
		rc = on_line_state;

		xSemaphoreGive(on_line_semaphore);
	}

	return rc;
}
