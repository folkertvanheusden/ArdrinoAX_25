#include "target.h"


class target_lora: public target
{
protected:
	std::optional<target_msg_t> wait_for_receive_packet() override;
	void send_message(const target_msg_t & msg) override;

public:
	target_lora(QueueHandle_t out, Print *const p, const int id, const int pin_nss, const int pin_reset, const int pin_dio0);
	virtual ~target_lora();
};
