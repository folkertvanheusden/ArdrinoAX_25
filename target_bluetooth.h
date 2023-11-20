#include "target.h"


class target_bluetooth: public target
{
private:
	bool has_device { false };

protected:
	std::optional<target_msg_t> wait_for_receive_packet() override;
	void send_message(const target_msg_t & msg) override;

public:
	target_bluetooth(QueueHandle_t out, Print *const p, const int id);
	virtual ~target_bluetooth();
};
