#include "i2sbuf_task.h"
#include "i2sbuf.h"

#include <stdlib.h>

#include "freertos/FreeRTOS.h"

#include "esp_log.h"
static const char *TAG = "i2sbuf_task";

void i2sbuf_task(void *args)
{
    i2sbuf_task_params_t config = *(i2sbuf_task_params_t*)args;

    // tell parent we have copied the params
    xTaskNotifyGive(config.parent_task);

    ESP_LOGD(TAG, "Allocating buffer");
    int16_t (*const buf)[2] = malloc(sizeof(*buf) * config.buf_len);

    size_t written;

    ESP_LOGD(TAG, "Started");

    for (;;)
    {
        config.callback(buf, config.buf_len, config.user_data);
        i2s_write(config.i2s_port, buf, sizeof(*buf) * config.buf_len, &written, portMAX_DELAY);
    }

    ESP_LOGD(TAG, "Terminating");

    free(buf);
    vTaskDelete(NULL);
}
