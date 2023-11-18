// this software is (C) 2016-2023 by folkert@vanheusden.com
// AGPL v3.0

#include <stdint.h>
#include <string>
#include <vector>


class kiss {
private:
	const size_t max_packet_size;

	bool (* const peek_radio)();
	void (* const get_radio)(uint8_t *const where_to, size_t *const n);
	void (* const put_radio)(const uint8_t *const what, const size_t size);
	size_t (* const peek_serial)();
	bool (* const get_serial)(uint8_t *const where_to, const size_t n, const uint32_t to);
	void (* const put_serial)(const uint8_t *const what, const size_t size);
	bool (* const reset_radio)();
	void (* const debug_out)(const std::string & what);

	void process_radio();
	void process_serial();

public:
	kiss(const size_t max_packet_size,
			bool (* const peek_radio)(), void (* const get_radio)(uint8_t *const where_to, size_t *const n), void (* const put_radio)(const uint8_t *const what, const size_t size),
			size_t (*const peek_serial)(), bool (* const get_serial)(uint8_t *const where_to, const size_t n, const uint32_t to), void (*const put_serial)(const uint8_t *const what, const size_t size),
			bool (*const reset_radio)(),
			void (* debug_out)(const std::string & what));
	~kiss();

	void debug(const std::string & msg);

	void begin();
	void loop();
};
