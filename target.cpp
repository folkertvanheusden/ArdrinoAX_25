#include "target.h"


void target_task(void *par)
{
	target *t = reinterpret_cast<target *>(par);

	for(;;)
		t->receive_task();
}

target::target(QueueHandle_t out, Print *const p, const int id) : out(out), p(p), id(id)
{
	BaseType_t xReturned = xTaskCreate(
                    target_task,     /* Function that implements the task. */
                    "target",        /* Text name for the task. */
                    3072,            /* Stack size in words, not bytes. */
                    (void *)this,    /* Parameter passed into the task. */
                    tskIDLE_PRIORITY,/* Priority at which the task is created. */
                    &task);          /* Used to pass out the created task's handle. */

	if (xReturned != pdPASS)
		p->println(F("Failed to create \"target\" task"));
}

target::~target()
{
	// ideally the task will now be stopped
}

void target::queue_message(const target_msg_t & what)
{
	if (what.target_id == id)
		return;

	send_message(what);
}

void target::receive_task()
{
	for(;;) {
		auto msg = wait_for_receive_packet();

		// send to scheduler
		if (msg.has_value())
			xQueueSend(out, &msg.value(), portMAX_DELAY);
		else if (is_poll)
			vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
