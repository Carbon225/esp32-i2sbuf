#include "i2sbuf.h"
#include "i2sbuf_internal.h"
#include "i2sbuf_generator.h"
#include "i2sbuf_writer.h"
#include "i2sbuf_buffers.h"
#include "freertos/task.h"

#include "esp_log.h"
static const char *TAG = "i2sbuf";

esp_err_t i2sbuf_install(
		gpio_num_t ws_io,
		gpio_num_t do_io,
		gpio_num_t clk_io,
		int sample_rate)
{
	ESP_LOGD(TAG, "Initializing");

	i2s_config_t i2s_config = {
			.mode = I2S_MODE_MASTER | I2S_MODE_TX,
			.sample_rate = sample_rate,
			.bits_per_sample = 16,
			.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
			.communication_format = I2S_COMM_FORMAT_I2S,
			.dma_buf_count = I2SBUF_DMA_COUNT,
			.dma_buf_len = I2SBUF_DMA_LEN, // in words
			.use_apll = 0,
			.intr_alloc_flags = ESP_INTR_FLAG_LEVEL3
	};

	i2s_pin_config_t pin_config = {
			.bck_io_num = clk_io,
			.ws_io_num = ws_io,
			.data_out_num = do_io,
			.data_in_num = GPIO_NUM_NC
	};

	ERROR_CHECK_RETURN(i2s_driver_install(I2SBUF_PORT, &i2s_config, 0, NULL));
	ERROR_CHECK_RETURN(i2s_set_pin(I2SBUF_PORT, &pin_config));

	buffer1_handle.writeSemaphore = xSemaphoreCreateBinary();
	generationQueue = xQueueCreate(2, sizeof(i2sbuf_buffer_t));

	// TODO check if created successfully

	// TODO proper stack size and priority
	xTaskCreatePinnedToCore(i2sbuf_writer_task, "i2swriter", 4096, NULL, 10, NULL, PRO_CPU_NUM);
	xTaskCreatePinnedToCore(i2sbuf_generator_task, "i2sgen1", 4096, NULL, 12, NULL, APP_CPU_NUM);
	ESP_LOGD(TAG, "Tasks spawned");

	ESP_LOGD(TAG, "Giving buffer handles");
	// TODO check for errors
	xQueueSend(generationQueue, &buffer1_handle, portMAX_DELAY);

	return ESP_OK;
}
