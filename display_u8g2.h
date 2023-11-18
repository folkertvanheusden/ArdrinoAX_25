#include "display.h"


class display_u8g2: public display
{
private:
	U8G2 *const physical_device;

public:
	display_u8g2(U8G2 *const physical_device);
	virtual ~display_u8g2();

	void refresh_physical() override;
	void set_powersave() override;
};

display *create_lilygo_display();
