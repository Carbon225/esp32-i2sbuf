#include "i2sbuf_generator.h"
#include "i2sbuf_buffers.h"
#include "i2sbuf.h"

#include "esp_log.h"
static const char *TAG = "i2sgen";

void i2sbuf_generator_task(void *args)
{
	i2sbuf_buffer_t currentBuffer;
	while (1)
	{
		ESP_LOGD(TAG, "Generator 1 waiting");
		if (xQueueReceive(generationQueue, &currentBuffer, portMAX_DELAY) == pdTRUE)
		{
			ESP_LOGD(TAG, "Generator 1 woken");

			// TODO null check buffer?

			// user has to know to write both channels (buf size * 2)
			i2sbuf_generate_samples(currentBuffer.buf, I2SBUF_BUF_SAMPLES);

			ESP_LOGD(TAG, "Generator 1 done");
			xSemaphoreGive(currentBuffer.writeSemaphore);
		}
	}
	vTaskDelete(NULL);
}
