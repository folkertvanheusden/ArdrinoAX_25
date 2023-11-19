#include <errno.h>
#include <string.h>

#include "target_udp.h"
#include "wifi.h"


target_udp::target_udp(QueueHandle_t out, Print *const p, const int id, const int port, const std::optional<std::string> & dest_addr):
	target(out, p, id),
	dest_addr(dest_addr)
{
	while(get_wifi_on_line() == false)
		vTaskDelay(100 / portTICK_PERIOD_MS);

	fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (fd == -1) {
		p->println(F("Cannot create UDP socket"));
		return;
	}

	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1)
		p->println(F("setsockopt failed"));

	sockaddr_in listen_addr { 0 };
	listen_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	listen_addr.sin_family      = AF_INET;
	listen_addr.sin_port        = htons(port);
	if (bind(fd, reinterpret_cast<const sockaddr *>(&listen_addr), sizeof(listen_addr)) == -1) {
		p->println(F("bind socket failed"));
		p->println(strerror(errno));
		return;
	}

	if (dest_addr.has_value()) {
		if (inet_aton(dest_addr.value().c_str(), &reinterpret_cast<sockaddr_in *>(&dest_sockaddr)->sin_addr.s_addr) == 0) {
			p->println(F("dest UDP addr invalid"));
			this->dest_addr.reset();
		}
		else {
			reinterpret_cast<sockaddr_in *>(&dest_sockaddr)->sin_family = AF_INET;
			reinterpret_cast<sockaddr_in *>(&dest_sockaddr)->sin_port   = htons(port);
		}
	}
}

target_udp::~target_udp()
{
}

std::optional<target_msg_t> target_udp::wait_for_receive_packet()
{
	if (fd == -1)
		return { };

	target_msg_t msg;
	msg.target_id = id;
	msg.data      = new std::vector<uint8_t>(1500);

	sockaddr_storage source_addr;
        socklen_t        socklen = sizeof(source_addr);

	int len = recvfrom(fd, msg.data->data(), msg.data->size(), 0, reinterpret_cast<sockaddr *>(&source_addr), &socklen);
	if (len == -1) {
		p->println(F("UDP recv failed"));
		delete msg.data;
		return { };
	}

	// TODO verify source address

	return { msg };
}

void target_udp::send_message(const target_msg_t & msg)
{
	if (dest_addr.has_value()) {
		int sfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		if (sendto(sfd, msg.data->data(), msg.data->size(), 0, reinterpret_cast<const sockaddr *>(&dest_sockaddr), sizeof(sockaddr_in)) == -1) {
			p->println(F("UDP send failed"));
			p->println(strerror(errno));
		}

		close(sfd);
	}
}
