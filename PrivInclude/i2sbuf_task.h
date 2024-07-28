#ifndef _COMPONENTS_I2SBUF_PRIVINCLUDE_I2SBUF_TASK_H
#define _COMPONENTS_I2SBUF_PRIVINCLUDE_I2SBUF_TASK_H_

#include "i2sbuf.h"
#include "driver/i2s_std.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

typedef struct i2sbuf_task_params
{
    i2s_chan_handle_t tx_chan;
    i2sbuf_callback_t callback;
    void *user_data;
    int buf_len;
    TaskHandle_t parent_task;
} i2sbuf_task_params_t;

void i2sbuf_task(void *args);

#endif
