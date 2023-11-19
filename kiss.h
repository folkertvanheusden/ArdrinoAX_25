#include <HardwareSerial.h>
#include <optional>
#include <stdint.h>
#include <vector>


std::vector<uint8_t>                 wrap_kiss  (const std::vector<uint8_t> & in);
std::optional<std::vector<uint8_t> > unwrap_kiss(const std::vector<uint8_t> & in);
std::vector<uint8_t>                 wait_for_kiss(HardwareSerial & s);
