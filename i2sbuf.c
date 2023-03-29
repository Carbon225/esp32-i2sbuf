#include "i2sbuf.h"

#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"


typedef struct i2sbuf_task_params
{
	i2s_chan_handle_t chan;
	i2sbuf_callback_t callback;
	void *user_data;
	int buf_len;
	void *buf;
	int buf_size;
} i2sbuf_task_params_t;


static void i2sbuf_task(void *args)
{
	i2sbuf_task_params_t *params = (i2sbuf_task_params_t*) args;
	size_t written;
	for (;;)
	{
		params->callback(params->buf, params->buf_len, params->user_data);
		i2s_channel_write(params->chan, params->buf, params->buf_size, &written, portMAX_DELAY);
	}
}

esp_err_t i2sbuf_install(const i2sbuf_config_t *config)
{
	i2s_chan_handle_t tx_chan;
	i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
	tx_chan_cfg.auto_clear = true;
	ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_chan, NULL));

    i2s_pdm_tx_config_t pdm_tx_cfg = {
        .clk_cfg = I2S_PDM_TX_CLK_DEFAULT_CONFIG(config->sample_rate),
        .slot_cfg = I2S_PDM_TX_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, config->stereo ? I2S_SLOT_MODE_STEREO : I2S_SLOT_MODE_MONO),
        .gpio_cfg = {
            .clk = config->clk_io,
            .dout = config->do_io,
            .invert_flags = {
                .clk_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_pdm_tx_mode(tx_chan, &pdm_tx_cfg));

	ESP_ERROR_CHECK(i2s_channel_enable(tx_chan));

	i2sbuf_task_params_t *task_params = malloc(sizeof(*task_params));
	assert(task_params);
	task_params->chan = tx_chan;
	task_params->callback = config->callback;
	task_params->user_data = config->user_data;
	task_params->buf_len = config->buf_len;
	task_params->buf_size = config->buf_len * sizeof(int16_t) * (config->stereo ? 2 : 1);
	task_params->buf = malloc(task_params->buf_size);
	assert(task_params->buf);

	xTaskCreate(
		i2sbuf_task,
		"i2sbuf_task",
		CONFIG_I2SBUF_TASK_STACK_SIZE,
		task_params,
		CONFIG_I2SBUF_TASK_PRIORITY,
		NULL
	);

	return ESP_OK;
}
