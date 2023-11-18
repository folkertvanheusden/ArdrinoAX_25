#include <Arduino.h>

#include "display.h"


display::display(const uint8_t cols, const uint8_t rows) :
	cols(cols),
	rows(rows)
{
	for(uint8_t i=0; i<rows; i++)
		contents.push_back(std::string(cols, ' '));
}

display::~display()
{
}

void display::print(const char c)
{
	contents.at(y).at(x++) = c;

	if (x >= cols) {
		y++;
		x = 0;

		if (y >= rows) {
			contents.erase(contents.begin() + 0);
			contents.push_back(std::string(cols, ' '));
		}
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
