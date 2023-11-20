#include <BluetoothSerial.h>
#include "kiss.h"
#include "target_bluetooth.h"


BluetoothSerial SerialBT;

target_bluetooth::target_bluetooth(QueueHandle_t out, Print *const p, const int id, const std::string & pin) :
	target(out, p, id)
{
	SerialBT.setPin(pin.c_str());
	SerialBT.enableSSP();

	has_device = SerialBT.begin("ArdrinoAX.25");
	if (has_device)
		p->println(F("BT enabled"));
	else
		p->println(F("BT device missing"));
}

target_bluetooth::~target_bluetooth()
{
}

std::optional<target_msg_t> target_bluetooth::wait_for_receive_packet()
{
	if (!has_device)
		return { };

	auto msg       = wait_for_kiss(SerialBT);
	auto unwrapped = unwrap_kiss(msg);

	if (unwrapped.has_value() == false)
		return { };

	return { { id, new std::vector<uint8_t>(unwrapped.value()) } };
}

void target_bluetooth::send_message(const target_msg_t & msg)
{
	if (!has_device)
		return;

	auto wrapped = wrap_kiss(*msg.data);
	SerialBT.write(wrapped.data(), wrapped.size());
}
