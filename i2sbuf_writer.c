#include "i2sbuf_writer.h"
#include "i2sbuf.h"
#include "i2sbuf_buffers.h"

#include "esp_log.h"
static const char *TAG = "i2swriter";

void i2sbuf_writer_task(void *args)
{
	while (1)
	{
		ESP_LOGD(TAG, "Writer waiting");
		if (xSemaphoreTake(buffer1_handle.writeSemaphore, portMAX_DELAY) == pdTRUE)
		{
			ESP_LOGD(TAG, "Writer woken");

			size_t written;
			// TODO create macro for size in bytes
			// size in bytes, 2 bytes for int16, 4 for 2 channels
			i2s_write(I2S_NUM_0, buffer1_handle.buf, I2SBUF_BUF_SIZE * 2, &written, portMAX_DELAY);

			if (written != I2SBUF_BUF_SAMPLES * 4)
				ESP_LOGW(TAG, "Wrote %zu/%zu bytes", written, I2SBUF_BUF_SAMPLES * 4);

			ESP_LOGD(TAG, "Writer done");
			xQueueSend(generationQueue, &buffer1_handle, portMAX_DELAY);
		}
	}
	vTaskDelete(NULL);
}
