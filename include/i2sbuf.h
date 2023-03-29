#ifndef _COMPONENTS_I2SBUF_INCLUDE_I2SBUF_H_
#define _COMPONENTS_I2SBUF_INCLUDE_I2SBUF_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "driver/i2s_pdm.h"

typedef void (*i2sbuf_callback_t)(void *buf, int n_samples, void *user_data);

typedef struct i2sbuf_config
{
    gpio_num_t do_io;
    gpio_num_t clk_io;
    int sample_rate;
    bool stereo;
    int buf_len;
    i2sbuf_callback_t callback;
    void *user_data;
} i2sbuf_config_t;

esp_err_t i2sbuf_install(const i2sbuf_config_t *config);

#ifdef __cplusplus
}
#endif

#endif
