#include "i2sbuf.h"
#include "i2sbuf_internal.h"
#include "i2sbuf_task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "esp_log.h"
static const char *TAG = "i2sbuf";

esp_err_t i2sbuf_install(const i2sbuf_config_t *config)
{
	ESP_LOGD(TAG, "Initializing");

	i2s_config_t i2s_config = {
			.mode = I2S_MODE_MASTER | I2S_MODE_TX,
			.sample_rate = config->sample_rate,
			.bits_per_sample = 16,
			.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
			.communication_format = I2S_COMM_FORMAT_STAND_I2S,
			.dma_buf_count = config->buf_count,
			.dma_buf_len = CONFIG_I2SBUF_DMA_BUF_LEN,
			.use_apll = false,
			.intr_alloc_flags = ESP_INTR_FLAG_LEVEL3
	};

	i2s_pin_config_t pin_config = {
			.bck_io_num = config->clk_io,
			.ws_io_num = config->ws_io,
			.data_out_num = config->do_io,
			.data_in_num = I2S_PIN_NO_CHANGE
	};

	ERROR_CHECK_RETURN(i2s_driver_install(config->i2s_port, &i2s_config, 0, NULL));
	ERROR_CHECK_RETURN(i2s_set_pin(config->i2s_port, &pin_config));

	ESP_LOGD(TAG, "I2S configured");

	i2sbuf_config_t *config_ptr = malloc(sizeof(i2sbuf_config_t));
	*config_ptr = *config;

	xTaskCreate(i2sbuf_task, "i2sbuf_task", 4096, config_ptr, 10, NULL);

	ESP_LOGD(TAG, "Tasks spawned");

	return ESP_OK;
}
