#ifndef COMPONENTS_I2SBUF_INCLUDE_I2SBUF_H_
#define COMPONENTS_I2SBUF_INCLUDE_I2SBUF_H_

#include "freertos/FreeRTOS.h"
#include "driver/i2s.h"

#define I2SBUF_DMA_LEN (512)
#define I2SBUF_DMA_COUNT (6)
#define I2SBUF_BUF_SAMPLES (I2SBUF_DMA_LEN)
#define I2SBUF_BUF_SIZE (I2SBUF_BUF_SAMPLES * 2)
#define I2SBUF_PORT (I2S_NUM_0)

esp_err_t i2sbuf_install(
		gpio_num_t ws_io,
		gpio_num_t do_io,
		gpio_num_t clk_io,
		int sample_rate);

// user defined
void i2sbuf_generate_samples(int16_t buf[], int n_samples);

#endif /* COMPONENTS_I2SBUF_INCLUDE_I2SBUF_H_ */
