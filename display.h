#include <Print.h>
#include <stdint.h>
#include <vector>


class display: public Print
{
private:
	const uint8_t cols;
	const uint8_t rows;
	uint8_t       x { 0 };
	uint8_t       y { 0 };
	SemaphoreHandle_t semaphore { xSemaphoreCreateMutex() };

protected:
	unsigned long on_since   { 0 };
	uint16_t      last_write { 0 };
	std::vector<std::string> contents;

public:
	display(const uint8_t cols, const uint8_t rows);
	virtual ~display();

	size_t write(uint8_t val) override;
	size_t write(const uint8_t *buffer, size_t size) override;

	unsigned long screen_on_time() const;
	uint16_t      get_last_write() const;

	virtual void refresh_physical() = 0;  // shall also update on_since
	virtual void set_power_state(const bool on) = 0;
};
