#include "target.h"


class target_beacon: public target
{
private:
	std::string   callsign;
	char          callsign_ssid { 0 };
	std::string   beacon_text;
	unsigned long interval_ms   { 0 };
	unsigned long last_transmit { 0 };

protected:
	std::optional<target_msg_t> wait_for_receive_packet() override;
	void send_message(const target_msg_t & msg) override;

public:
	target_beacon(QueueHandle_t out, Print *const p, const int id, const std::string & callsign, const char callsign_ssid, const std::string & beacon_text, const unsigned long interval_ms);
	virtual ~target_beacon();
};
