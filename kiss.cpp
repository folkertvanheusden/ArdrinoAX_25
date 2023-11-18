// this software is (C) 2016-2023 by folkert@vanheusden.com
// AGPL v3.0

#include <Arduino.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "kiss.h"


kiss::kiss(const size_t max_packet_size,
		bool (* peek_radio)(), void (* get_radio)(uint8_t *const where_to, size_t *const n), void (* put_radio)(const uint8_t *const what, const size_t size),
		size_t (*peek_serial)(), bool (* get_serial)(uint8_t *const where_to, const size_t n, const uint32_t to), void (*put_serial)(const uint8_t *const what, const size_t size),
		bool (* reset_radio)(),
		void (* debug_out)(const std::string & what)):
	max_packet_size(max_packet_size),
	peek_radio(peek_radio),
	get_radio(get_radio),
	put_radio(put_radio),
	peek_serial(peek_serial),
	get_serial(get_serial),
	put_serial(put_serial),
	reset_radio(reset_radio),
	debug_out(debug_out)
{
	debug("START");
}

kiss::~kiss() {
}

void kiss::begin() {
}

#define FEND	0xc0
#define FESC	0xdb
#define TFEND	0xdc
#define TFESC	0xdd

void put_byte(std::vector<uint8_t> *const out, const uint8_t c)
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

void kiss::debug(const std::string & msg) {
	std::vector<uint8_t> ax25_ident { 0x92, 0x88, 0x8a, 0x9c, 0xa8, 0x40, 0xe0, 0x88, 0x8a, 0x84, 0xaa, 0x8e, 0x60, 0x63, 0x03, 0xf0 };

	debug_out(msg);

	std::vector<uint8_t> buffer_out;
	buffer_out.push_back(FEND);
	buffer_out.push_back(0x00);

	for(size_t i=0; i<ax25_ident.size(); i++)
		put_byte(&buffer_out, ax25_ident.at(i));

	for(size_t i=0; i<msg.size(); i++)
		put_byte(&buffer_out, msg.at(i));

	buffer_out.push_back(FEND);

	put_serial(buffer_out.data(), buffer_out.size());
}

void kiss::process_radio() {
	size_t               nBytes = max_packet_size;
	std::vector<uint8_t> buffer_in(nBytes);
	get_radio(buffer_in.data(), &nBytes);

	std::vector<uint8_t> buffer_out(nBytes);

	buffer_out.push_back(FEND);
	buffer_out.push_back(0x00);

	for(size_t i=0; i<nBytes; i++)
		put_byte(&buffer_out, buffer_in.at(i));

	buffer_out.push_back(FEND);

	put_serial(buffer_out.data(), buffer_out.size());
}

// when a byte comes in via serial, it is expected that a full kiss
// packet comes in. this method waits for that with a timeout of
// 2 seconds
void kiss::process_serial() {
	bool ok     = false;
	bool escape = false;

	const uint32_t end = millis() + 2000;

	std::vector<uint8_t> buffer_in;

	while(millis() <= end) {
		uint8_t buffer = 0;

		if (!get_serial(&buffer, 1, end)) {
			debug("ser recv to");
			break;
		}

		if (escape) {
			if (buffer_in.size() == max_packet_size) {
				debug("error packet size2");
				break;
			}

			if (buffer == TFEND)
				buffer_in.push_back(FEND);
			else if (buffer == TFESC)
				buffer_in.push_back(FESC);
			else {
				debug("error escape");
			}

			escape = false;
		}
		else if (buffer == FEND) {
			if (buffer_in.empty() == false) {
				ok = true;
				break;
			}
		}
		else if (buffer == FESC)
			escape = true;
		else {
			if (buffer_in.size() == max_packet_size) {
				debug("error packet size3");
				break;
			}

			buffer_in.push_back(buffer);
		}
	}

	if (ok) {
		if (buffer_in.empty()) {
			debug("no data");
			ok = false;
		}
		else {
			put_radio(buffer_in.data() + 1, buffer_in.size() - 1);
		}
	}

	debug(ok ? "OK": "FAIL");
}

void kiss::loop() {
	if (peek_radio())
		process_radio();

	if (peek_serial())
		process_serial();
}
