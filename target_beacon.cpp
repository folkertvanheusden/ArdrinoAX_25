#include <Arduino.h>

#include "ax25.h"
#include "kiss.h"
#include "target_beacon.h"


target_beacon::target_beacon(QueueHandle_t out, Print *const p, const int id, const std::string & callsign, const char callsign_ssid, const std::string & beacon_text, const unsigned long interval_ms) :
	target(out, p, id),
	callsign(callsign),
	callsign_ssid(callsign_ssid),
	beacon_text(beacon_text),
	interval_ms(interval_ms)
{
}

target_beacon::~target_beacon()
{
}

std::optional<target_msg_t> target_beacon::wait_for_receive_packet()
{
	unsigned long now = millis();

	if (last_transmit == 0 || now - last_transmit >= interval_ms) {
		last_transmit = now;

		ax25_packet a;
		a.set_from   (callsign, callsign_ssid, true, false);
		a.set_to     ("IDENT", 0, false, false);
		a.set_control(0x03);  // unnumbered information/frame
		a.set_data   (reinterpret_cast<const uint8_t *>(beacon_text.c_str()), beacon_text.size());
		a.set_pid    (0xf0);  // beacon

		auto ax25_packet = a.generate_packet();
		auto wrapped     = wrap_kiss(std::vector<uint8_t>(ax25_packet.first, ax25_packet.first + ax25_packet.second));
		free(ax25_packet.first);

		return { { id, new std::vector<uint8_t>(wrapped) } };
	}

	return { };
}

void target_beacon::send_message(const target_msg_t & msg)
{
}
