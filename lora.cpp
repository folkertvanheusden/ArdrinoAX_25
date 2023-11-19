#include <LoRa.h>
#include <optional>
#include <SPI.h>
#include <stdint.h>
#include <vector>

static void LoRa_rx_mode() {
	LoRa.receive();
}

static void LoRa_tx_mode() {
	LoRa.idle();
}

void put_lora(const std::vector<uint8_t> & data) {
	LoRa_tx_mode();

	LoRa.beginPacket();
	LoRa.write(data.data(), data.size());
	LoRa.endPacket();

	LoRa_rx_mode();
}

std::vector<uint8_t> * get_lora() {
	size_t n = LoRa.parsePacket();
	if (n == 0)
		return nullptr;

	std::vector<uint8_t> *out = new std::vector<uint8_t>();

	for(size_t i=0; i<n; i++)
		out->push_back(LoRa.read());

	return out;
}

// specifically for APRS
bool init_lora(const int pin_nss, const int pin_reset, const int pin_dio0) {
	LoRa.setPins(pin_nss, pin_reset, pin_dio0);

	if (!LoRa.begin(433775000l))
		return false;

	LoRa.setSpreadingFactor(12);
	LoRa.setSignalBandwidth(125000);
	LoRa.setCodingRate4(5);
	LoRa.setPreambleLength(8);
	LoRa.setSyncWord(0x12);
	LoRa.enableCrc();

	LoRa.setTxPower(20);

	LoRa_rx_mode();

	return true;
}
