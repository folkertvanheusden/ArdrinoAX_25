#include <Wire.h>

#include "display_u8g2.h"


display_u8g2::display_u8g2(U8G2 *const physical_device):
	display(physical_device->getCols(), physical_device->getRows()),
	physical_device(physical_device)
{
}

display_u8g2::~display_u8g2()
{
}

void display_u8g2::refresh_physical()
{
	physical_device->clearBuffer();
	physical_device->setFlipMode(0);
	physical_device->setDrawColor(1);
	physical_device->setFontDirection(0);
	physical_device->firstPage();
	physical_device->setFont(u8g_font_helvB08);

	do {
		for(size_t i=0; i<contents.size(); i++)
			physical_device->drawStr(0, 8 + 8 * i, contents.at(i).c_str());
	} while (physical_device->nextPage());

	physical_device->sendBuffer();
}

void display_u8g2::set_power_state(const bool on)
{
	if (on)
		on_since = millis();

	physical_device->setPowerSave(!on);
}

display *create_lilygo_display()
{
	constexpr int I2C_SDA = 21;
	constexpr int I2C_SCL = 22;
	Wire.begin(I2C_SDA, I2C_SCL);

	U8G2 *u8g2 = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);
	u8g2->begin();

	return new display_u8g2(u8g2);
}
