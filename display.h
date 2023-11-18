#include <stdint.h>
#include <vector>


class display
{
private:
	const uint8_t cols;
	const uint8_t rows;
	uint8_t x;
	uint8_t y;

protected:
	std::vector<std::string> contents;

public:
	display(const uint8_t cols, const uint8_t rows);
	virtual ~display();

	void print(const char c);
	void println(const std::string & str);
	void println(const String & str);

	virtual void refresh_physical() = 0;
	virtual void set_powersave() = 0;
};
