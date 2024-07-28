#include "i2sbuf.h"
#include "i2sbuf_task.h"

#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "sdkconfig.h"

#include "esp_log.h"
static const char *TAG = "i2sbuf";

esp_err_t i2sbuf_install(const i2sbuf_config_t *config)
{
    esp_err_t err;

    ESP_LOGD(TAG, "Initializing");

    i2s_config_t i2s_config = {
            .mode = I2S_MODE_MASTER | I2S_MODE_TX,
            .sample_rate = config->sample_rate,
            .bits_per_sample = 16,
            .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
            .communication_format = I2S_COMM_FORMAT_STAND_I2S,
            .dma_buf_count = config->buf_count,
            .dma_buf_len = config->buf_len,
            .use_apll = config->use_apll,
            .intr_alloc_flags = ESP_INTR_FLAG_LEVEL3,
    };

    i2s_pin_config_t pin_config = {
            .bck_io_num = config->clk_io,
            .ws_io_num = config->ws_io,
            .data_out_num = config->do_io,
            .data_in_num = I2S_PIN_NO_CHANGE,
    };

    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2s_driver_install(config->i2s_port, &i2s_config, 0, NULL));
    if (err) goto error;  // no cleanup

    err = ESP_ERROR_CHECK_WITHOUT_ABORT(i2s_set_pin(config->i2s_port, &pin_config));
    if (err) goto error_install;  // uninstall driver

    ESP_LOGD(TAG, "I2S configured");

    i2sbuf_task_params_t *task_params = malloc(sizeof(*task_params));
    task_params->i2s_port = config->i2s_port;
    task_params->callback = config->callback;
    task_params->user_data = config->user_data;
    task_params->buf_len = config->buf_len;
    task_params->parent_task = xTaskGetCurrentTaskHandle();

    TaskHandle_t child_task;

    ESP_LOGD(TAG, "Creating child task");

    if (xTaskCreate(i2sbuf_task,
                    "i2sbuf_task",
                    CONFIG_I2SBUF_TASK_STACK_SIZE,
                    task_params,
                    CONFIG_I2SBUF_TASK_PRIORITY,
                    &child_task) != pdPASS)
    {
        ESP_LOGE(TAG, "Failed to spawn task");
        err = ESP_FAIL;
        goto error_params;  // free task params
    }

    ESP_LOGD(TAG, "Tasks spawned");

    // wait for task to receive params
    if (!ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(CONFIG_I2SBUF_CREATE_TASK_TIMEOUT_MS
                                                ? CONFIG_I2SBUF_CREATE_TASK_TIMEOUT_MS
                                                : portMAX_DELAY)))
    {
        ESP_LOGE(TAG, "Created task not responding");
        goto error_task;  // delete task
    }

    ESP_LOGD(TAG, "Task is running");

    // task copied params, free original
    free(task_params);
    return ESP_OK;

    error_task:
    vTaskDelete(child_task);
    error_params:
    free(task_params);
    error_install:
    i2s_driver_uninstall(config->i2s_port);
    error:
    return err;
}
