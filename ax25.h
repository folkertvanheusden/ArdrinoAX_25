// (C) 2022-2023 by folkert van heusden <mail@vanheusden.com>, released under Apache License v2.0
#pragma once

#include <optional>
#include <stdint.h>
#include <string>
#include <vector>

#include "str.h"


class ax25_address
{
private:
	bool        valid    { false };
	std::string invalid_reason;
	std::string address;
	int         ssid     { 0     };
	bool        end_mark { false };
	bool        repeated { false };

public:
	ax25_address();
	ax25_address(const std::vector<uint8_t> & from);
	ax25_address(const ax25_address & a);
	ax25_address(const std::string & a, const int ssid, const bool end_mark, const bool repeated);
	ax25_address(const std::string & a, const bool end_mark, const bool repeated);

	ax25_address & operator=(const ax25_address &);
	bool operator==(const ax25_address & other) const;

	bool get_valid()    const { return valid;    }
	std::string get_invalid_reason() const { return invalid_reason; }

	bool get_end_mark()   const { return end_mark;  }
	bool get_repeated()   const { return repeated;  }
	void reset_end_mark()       { end_mark = false; }
	void set_end_mark()         { end_mark = true;  }

	std::string get_address() const { return address; }
	std::string to_str()      const { return address + myformat("-%d", ssid); }
	int         get_ssid()    const { return ssid; }

	void set_address(const std::string & address, const int ssid);

	std::vector<uint8_t> generate_address() const;
};

class ax25_packet
{
public:
	enum frame_type { TYPE_I, TYPE_S, TYPE_U, TYPE_UI };

private:
	bool                      valid    { false };
	std::string               invalid_reason;
	ax25_address              from;
	ax25_address              to;
	std::vector<ax25_address> repeaters;
	uint8_t                   control  { 0     };
	frame_type                type     { TYPE_I };
	std::optional<uint8_t>    msg_nr   {       };
	std::optional<uint8_t>    pid      {       };
	std::vector<uint8_t>      data;

public:
	ax25_packet();
	ax25_packet(const std::vector<uint8_t> & in);
	~ax25_packet();

	void set_from    (const std::string & callsign, const int ssid, const bool end_mark, const bool repeated);
	void set_to      (const std::string & callsign, const int ssid, const bool end_mark, const bool repeated);
	void set_control (const uint8_t control);
	void set_pid     (const uint8_t pid    );
	void set_type    (const frame_type f   );
	void set_data    (const uint8_t *const p, const size_t size);

	void add_repeater(const ax25_address & addr);

	ax25_address get_from() const;
	ax25_address get_to  () const;
	std::vector<ax25_address> get_repeaters() const;
	auto         get_data() const;
	std::optional<uint8_t> get_pid () const;
	frame_type   get_type() const  { return type;  }
	bool         get_valid() const { return valid; }
	std::string  get_invalid_reason() const { return invalid_reason; }

	std::pair<uint8_t *, size_t> generate_packet() const;

	std::string  to_str() const;
};
