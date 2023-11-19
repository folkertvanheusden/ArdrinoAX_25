#include <lwip/err.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
#include <lwip/sys.h>

#include "target.h"


class target_udp: public target
{
private:
	int fd { -1 };
	std::optional<std::string> dest_addr;
	sockaddr_storage dest_sockaddr { 0 };

protected:
	std::optional<target_msg_t> wait_for_receive_packet() override;
	void send_message(const target_msg_t & msg) override;

public:
	target_udp(QueueHandle_t out, Print *const p, const int id, const int port, const std::optional<std::string> & dest_addr);
	virtual ~target_udp();
};
