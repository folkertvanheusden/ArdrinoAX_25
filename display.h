#include <stdint.h>
#include <vector>


class display
{
private:
	const uint8_t cols;
	const uint8_t rows;
	uint8_t       x { 0 };
	uint8_t       y { 0 };

protected:
	unsigned long on_since { 0 };
	std::vector<std::string> contents;

public:
	display(const uint8_t cols, const uint8_t rows);
	virtual ~display();

	void print(const char c);
	void println(const std::string & str);
	void println(const String & str);
	void println(const char *const str);

	unsigned long screen_on_time() const;

	virtual void refresh_physical() = 0;  // shall also update on_since
	virtual void set_powersave() = 0;
};
