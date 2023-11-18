#include <configure_wifi.h>
#include <LittleFS.h>
#include <start_wifi.h>


static bool progress_indicator(const int nr, const int mx, const std::string & which) {
	return true;
}

static void wifi_task(void *) {
	set_hostname("ArdrinoAX.25");

	enable_wifi_debug();

	for(;;) {
		scan_access_points_start();

		configure_wifi cw;

		if (cw.is_configured() == false) {
			start_wifi("ArdrinoAX.25");  // enable wifi with AP (empty string for no AP)

			cw.configure_aps();
		}
		else {
			start_wifi("");
		}

		// see what we can see
		scan_access_points_start();

		while(scan_access_points_wait() == false)
			vTaskDelay(100 / portTICK_PERIOD_MS);

		auto available_access_points = scan_access_points_get();

		// try to connect
		auto state = try_connect_init(cw.get_targets(), available_access_points, 300, progress_indicator);
		connect_status_t cs = CS_IDLE;

		unsigned long end_ts = millis() + 60000l;  // 60s wait

		while(millis() < end_ts) {
			cs = try_connect_tick(state);

			if (cs != CS_IDLE)
				break;

			vTaskDelay(100 / portTICK_PERIOD_MS);
		}

		WiFi.setSleep(false);

		while(check_wifi_connection_status() == CS_CONNECTED)
			vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

bool start_wifi() {
	if (!LittleFS.begin())
		return false;

	TaskHandle_t xHandle   = nullptr;
	BaseType_t   xReturned = xTaskCreate(
                    wifi_task,       /* Function that implements the task. */
                    "wifi",          /* Text name for the task. */
                    8192,            /* Stack size in words, not bytes. */
                    nullptr,         /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle);       /* Used to pass out the created task's handle. */

	return xReturned == pdPASS;
}
