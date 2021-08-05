#include "i2sbuf_task.h"
#include "i2sbuf.h"

#include "sdkconfig.h"

#if CONFIG_I2SBUF_BUFFER_ON_HEAP
#include "stdlib.h"
#endif

#include "esp_log.h"
static const char *TAG = "i2sbuf_task";

void i2sbuf_task(void *args)
{
	i2sbuf_task_params_t config = *(i2sbuf_task_params_t*)args;
	free(args);

#if CONFIG_I2SBUF_BUFFER_ON_HEAP
	ESP_LOGD(TAG, "Dynamically allocating buffer");
	int16_t (*const buf)[2] = malloc(sizeof(*buf) * CONFIG_I2SBUF_DMA_BUF_LEN);
#else
	ESP_LOGD(TAG, "Using stack buffer");
	int16_t buf[CONFIG_I2SBUF_DMA_BUF_LEN][2];
#endif

	size_t written;

	ESP_LOGD(TAG, "Started");

	for (;;)
	{
		config.callback(buf, CONFIG_I2SBUF_DMA_BUF_LEN, config.user_data);
		i2s_write(config.i2s_port, buf, sizeof(*buf) * CONFIG_I2SBUF_DMA_BUF_LEN, &written, portMAX_DELAY);
	}

#if CONFIG_I2SBUF_BUFFER_ON_HEAP
	free(buf);
#endif
}
