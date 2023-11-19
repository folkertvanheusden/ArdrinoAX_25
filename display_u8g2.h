#include <U8g2lib.h>
#include "display.h"


class display_u8g2: public display
{
private:
	U8G2 *const physical_device;

public:
	display_u8g2(U8G2 *const physical_device);
	virtual ~display_u8g2();

	void refresh_physical() override;
	void set_power_state(const bool on) override;
};

display *create_lilygo_display();
