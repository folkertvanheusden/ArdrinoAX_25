#include "lora.h"

bool init_lora(const int pin_nss, const int pin_reset, const pin_dio0);
void put_lora(const std::vector<uint8_t> & data);
std::optional<std::vector<uint8_t> > get_lora();
