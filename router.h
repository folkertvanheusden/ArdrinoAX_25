#include <Print.h>
#include <vector>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

#include "target.h"


void router(const QueueHandle_t q, Print *const p, std::vector<target *> *const targets);
