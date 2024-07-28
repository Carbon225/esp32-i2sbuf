#include "i2sbuf.h"
#include "i2sbuf_task.h"

#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"

#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "i2sbuf";

void i2sbuf_install(const i2sbuf_config_t *config)
{
    ESP_LOGD(TAG, "Initializing");

    i2s_chan_handle_t tx_chan;
    i2s_chan_config_t tx_chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_AUTO, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&tx_chan_cfg, &tx_chan, NULL));

    i2s_std_config_t tx_std_cfg = {
        .clk_cfg  = I2S_STD_CLK_DEFAULT_CONFIG(config->sample_rate),
        .slot_cfg = I2S_STD_MSB_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_GPIO_UNUSED,
            .bclk = config->clk_io,
            .ws   = config->ws_io,
            .dout = config->do_io,
            .din  = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv   = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(tx_chan, &tx_std_cfg));

    ESP_LOGD(TAG, "I2S configured");

    i2sbuf_task_params_t task_params;
    task_params.tx_chan = tx_chan;
    task_params.callback = config->callback;
    task_params.user_data = config->user_data;
    task_params.buf_len = config->buf_len;
    task_params.parent_task = xTaskGetCurrentTaskHandle();

    TaskHandle_t child_task;

    ESP_LOGD(TAG, "Creating child task");

    if (xTaskCreate(i2sbuf_task,
                    "i2sbuf_task",
                    CONFIG_I2SBUF_TASK_STACK_SIZE,
                    &task_params,
                    CONFIG_I2SBUF_TASK_PRIORITY,
                    &child_task) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to spawn task");
        ESP_ERROR_CHECK(ESP_FAIL);
    }

    ESP_LOGD(TAG, "Tasks spawned");

    if (!ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(CONFIG_I2SBUF_CREATE_TASK_TIMEOUT_MS
                                                ? CONFIG_I2SBUF_CREATE_TASK_TIMEOUT_MS
                                                : portMAX_DELAY)))
    {
        ESP_LOGE(TAG, "Created task not responding");
        ESP_ERROR_CHECK(ESP_FAIL);
    }

    ESP_LOGD(TAG, "Task is running");
}
