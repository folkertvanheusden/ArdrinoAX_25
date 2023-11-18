#include <Arduino.h>

#include "display.h"


static void display_idle_task(void *d) {
	display *disp = reinterpret_cast<display *>(d);

	for(;;) {
		if (disp->screen_on_time() >= 15000)
			disp->set_powersave();

		vTaskDelay(500 / portTICK_PERIOD_MS);
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

void display::print(const char c)
{
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
	}
}

void display::println(const std::string & str)
{
	for(size_t i=0; i<str.size(); i++)
		print(str.at(i));
}

void display::println(const String & str)
{
	for(unsigned i=0; i<str.length(); i++)
		print(str.charAt(i));
}

void display::println(const char *const str)
{
	size_t len = strlen(str);

	for(unsigned i=0; i<len; i++)
		print(str[i]);
}

unsigned long display::screen_on_time() const
{
	return millis() - on_since;
}
