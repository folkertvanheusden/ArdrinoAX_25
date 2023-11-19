#include <Arduino.h>

#include "display.h"


static void display_idle_task(void *d) {
	display *disp = reinterpret_cast<display *>(d);

	uint16_t last_write = 0;

	for(;;) {
		uint16_t temp = disp->get_last_write();
		if (temp != last_write) {
			last_write = temp;

			disp->refresh_physical();
			disp->set_power_state(true);
		}

		vTaskDelay(200 / portTICK_PERIOD_MS);

		if (disp->screen_on_time() >= 15000)
			disp->set_power_state(false);
	}
}

display::display(const uint8_t cols, const uint8_t rows) :
	cols(cols),
	rows(rows)
{
	for(uint8_t i=0; i<rows; i++)
		contents.push_back(std::string(cols, ' '));

	TaskHandle_t xHandle = nullptr;
	BaseType_t xReturned = xTaskCreate(
                    display_idle_task,/* Function that implements the task. */
                    "screensaver",   /* Text name for the task. */
                    2048,            /* Stack size in words, not bytes. */
                    this,            /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &xHandle);       /* Used to pass out the created task's handle. */
}

display::~display()
{
}

size_t display::write(uint8_t c)
{
Serial.print(char(c));  // TODO
	if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
		if (c == 13)
			x = 0;
		else if (c == 10)
			y++;
		else
			contents.at(y).at(x++) = c;

		if (x >= cols) {
			y++;
			x = 0;
		}

		if (y >= rows) {
			contents.erase(contents.begin() + 0);
			contents.push_back(std::string(cols, ' '));
			y = rows - 1;
			x = 0;
		}

		last_write++;

		xSemaphoreGive(semaphore);
	}

	return 1;
}

size_t display::write(const uint8_t *buffer, size_t size)
{
	for(size_t i=0; i<size; i++)
		write(buffer[i]);

	return size;
}

unsigned long display::screen_on_time() const
{
	return millis() - on_since;
}

uint16_t display::get_last_write() const
{
	uint16_t rc = 0;

	if (xSemaphoreTake(semaphore, portMAX_DELAY) == pdTRUE) {
		rc = last_write;

		xSemaphoreGive(semaphore);
	}

	return rc;
}
