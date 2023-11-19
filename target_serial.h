#include "target.h"


class target_serial: public target
{
protected:
	std::optional<target_msg_t> wait_for_receive_packet() override;
	void send_message(const target_msg_t & msg) override;

public:
	target_serial(QueueHandle_t out, Print *const p);
	virtual ~target_serial();
};
