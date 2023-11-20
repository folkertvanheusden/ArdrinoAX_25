#include "gps.h"
#include "target.h"


class target_aprs: public target
{
private:
	std::string   callsign;
	char          callsign_ssid { 0       };
	gps          *g             { nullptr };
	std::string   beacon_text;
	unsigned long interval_ms   { 0       };
	unsigned long last_transmit { 0       };
	std::optional<double> send_every_x_meter;

protected:
	std::optional<target_msg_t> wait_for_receive_packet() override;
	void send_message(const target_msg_t & msg) override;

public:
	target_aprs(QueueHandle_t out, Print *const p, const int id, const std::string & callsign, const char callsign_ssid, gps *const g, const std::string & beacon_text, const unsigned long interval_ms, const std::optional<double> send_every_x_meter);
	virtual ~target_aprs();
};
