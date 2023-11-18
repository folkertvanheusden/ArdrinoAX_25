#include <U8g2lib.h>

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
	physical_device->setFont(u8g2_font_helvB12_tf);

	do {
		for(size_t i=0; i<contents.size(); i++)
			physical_device->drawStr(0, 12 + 13 * i, contents.at(i).c_str());
	} while (physical_device->nextPage());

	physical_device->setPowerSave(0);

	physical_device->sendBuffer();
}

void display_u8g2::set_powersave()
{
	physical_device->setPowerSave(1);
}
