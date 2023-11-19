#include "kiss.h"


constexpr uint8_t FEND  = 0xc0;
constexpr uint8_t FESC  = 0xdb;
constexpr uint8_t TFEND = 0xdc;
constexpr uint8_t TFESC = 0xdd;

static void put_byte(std::vector<uint8_t> *const out, const uint8_t c)
{
	if (c == FEND) {
		out->push_back(FESC);
		out->push_back(TFEND);
	}
	else if (c == FESC) {
		out->push_back(FESC);
		out->push_back(TFESC);
	}
	else {
		out->push_back(c);
	}
}

std::vector<uint8_t> wrap_kiss(const std::vector<uint8_t> & in)
{
	std::vector<uint8_t> out;

	out.push_back(FEND);
	out.push_back(0x00);

	for(auto & byte: in)
		put_byte(&out, byte);

	out.push_back(FEND);

	return std::move(out);
}

std::optional<std::vector<uint8_t> > unwrap_kiss(const std::vector<uint8_t> & in)
{
	bool escape = false;

	std::vector<uint8_t> out;

	for(auto & buffer: in) {
		if (escape) {
			if (buffer == TFEND)
				out.push_back(FEND);
			else if (buffer == TFESC)
				out.push_back(FESC);
			else {
				return { };
			}

			escape = false;
		}
		else if (buffer == FEND) {
			if (out.empty() == false)
				return { std::move(out) };
		}
		else if (buffer == FESC)
			escape = true;
		else {
			out.push_back(buffer);
		}
	}

	if (escape)  // wrong mode
		return { };

	if (out.size() == 1)  // command byte missing
		return { };

	out.erase(out.begin() + 0);

	return { std::move(out) };
}

std::vector<uint8_t> wait_for_kiss(HardwareSerial & s)
{
	std::vector<uint8_t> out;

	for(;;) {
		if (s.available() == 0) {
			vTaskDelay(10 / portTICK_PERIOD_MS);
			continue;
		}

		int c = s.read();

		out.push_back(uint8_t(c));

		if (c == FEND)
			break;

		if (out.size() > 1024)
			out.clear();
	}

	return out;
}
