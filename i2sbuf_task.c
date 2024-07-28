#include "i2sbuf_task.h"
#include "i2sbuf.h"

#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "esp_log.h"
#include "esp_err.h"

static const char *TAG = "i2sbuf_task";

void i2sbuf_task(void *args)
{
    i2sbuf_task_params_t config = *(i2sbuf_task_params_t*)args;

    xTaskNotifyGive(config.parent_task);

    ESP_LOGD(TAG, "Allocating buffer");
    size_t buf_len_samples = config.buf_len;
    size_t buf_len_bytes = sizeof(int16_t[2]) * buf_len_samples;
    uint8_t *const buf = malloc(buf_len_bytes);

    ESP_ERROR_CHECK(i2s_channel_enable(config.tx_chan));

    ESP_LOGD(TAG, "Started");

    for (;;)
    {
        config.callback((int16_t(*)[2])buf, buf_len_samples, config.user_data);
        size_t bytes_sent = 0;
        while (bytes_sent < buf_len_bytes)
        {
            size_t bytes_written;
            ESP_ERROR_CHECK(i2s_channel_write(config.tx_chan, buf + bytes_sent, buf_len_bytes - bytes_sent, &bytes_written, 1000));
            bytes_sent += bytes_written;
        }
    }

    ESP_LOGD(TAG, "Terminating");

    free(buf);
    vTaskDelete(NULL);
}
