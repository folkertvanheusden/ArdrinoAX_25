#include <Arduino.h>

#include "ax25.h"
#include "kiss.h"
#include "target_aprs.h"


target_aprs::target_aprs(QueueHandle_t out, Print *const p, const int id, const std::string & callsign, const char callsign_ssid, gps *const g, const std::string & beacon_text, const unsigned long interval_ms, const std::optional<double> send_every_x_meter, const bool oe) :
	target(out, p, id),
	callsign(callsign),
	callsign_ssid(callsign_ssid),
	g(g),
	beacon_text(beacon_text),
	interval_ms(interval_ms),
	send_every_x_meter(send_every_x_meter),
	send_oe(oe)
{
}

target_aprs::~target_aprs()
{
}

std::optional<target_msg_t> target_aprs::wait_for_receive_packet()
{
	unsigned long now = millis();

	auto position = g->get_position();
	if (position.has_value() == false)
		return { };

	bool send_message = last_transmit == 0;
	send_message |= now - last_transmit >= interval_ms;

	auto speed = g->get_speed();
	if (send_every_x_meter.has_value() && speed.has_value()) {
		double meter_per_second = speed.value() / 3.6;
		unsigned long max_interval = (send_every_x_meter.value() / meter_per_second) * 1000;

		send_message |= now - last_transmit >= max_interval;
	}

	if (send_message) {
		last_transmit = now;
		std::string aprs_message = "!" + gps_double_to_aprs(position.value().first, position.value().second);

		if (beacon_text.empty() == false)
			aprs_message += "[" + beacon_text;

		if (send_oe) {
			std::string packet = "\x3c\xff\x01APLG01>" + callsign + myformat("-%d", callsign_ssid) + ":" + aprs_message;

			return { { id, new std::vector<uint8_t>(packet.data(), packet.data() + packet.size()) } };
		}

		// if not OE, wrap in a AX.25 beacon message
		ax25_packet a;
		a.set_from   (callsign, callsign_ssid, true, false);
		a.set_to     ("APLG01", 0, false, false);
		a.set_control(0x03);  // unnumbered information/frame
		a.set_data   (reinterpret_cast<const uint8_t *>(aprs_message.c_str()), aprs_message.size());
		a.set_pid    (0xf0);  // beacon

		auto ax25_packet = a.generate_packet();
		auto wrapped     = wrap_kiss(std::vector<uint8_t>(ax25_packet.first, ax25_packet.first + ax25_packet.second));
		free(ax25_packet.first);

		return { { id, new std::vector<uint8_t>(wrapped) } };
	}

	return { };
}

void target_aprs::send_message(const target_msg_t & msg)
{
}
