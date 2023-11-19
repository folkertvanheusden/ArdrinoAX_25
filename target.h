#pragma once
#include <optional>
#include <Print.h>
#include <stdint.h>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>


typedef struct {
	int target_id;
	std::vector<uint8_t> data;
} target_msg_t;

class target
{
protected:
	QueueHandle_t out     { nullptr };
	TaskHandle_t  task    { nullptr };
	Print        *const p { nullptr };
	int           id      { 0       };
	const bool    is_poll { true    };

	virtual std::optional<target_msg_t> wait_for_receive_packet() = 0;
	virtual void send_message(const target_msg_t & msg) = 0;

public:
	target(QueueHandle_t out, Print *const p, const int id);
	virtual ~target();

	void queue_message(const target_msg_t & what);
	void receive_task();
};
