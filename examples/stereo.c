#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "i2sbuf.h"

#include "esp_log.h"
static const char *TAG = "main";

#define I2S_NUM (I2S_NUM_0)
#define I2S_WS (GPIO_NUM_16)
#define I2S_DO (GPIO_NUM_17)
#define I2S_CK (GPIO_NUM_5)

#define SAMPLE_RATE (48000)

#define BUF_LEN (256)
#define BUF_COUNT (4)

typedef struct wave_state
{
    float freq;
    float phase;
} wave_state_t;

void audio_callback(int16_t buf[][2], int n_samples, void *user_data)
{
    wave_state_t *state = (wave_state_t*) user_data;
    for (int i = 0; i < n_samples; ++i)
    {
        float sample = sinf(state->phase * (float) M_PI);

        buf[i][0] = (int16_t) (sample * (float) INT16_MAX);
        buf[i][1] = (int16_t) (state->phase * (float) INT16_MAX);

        state->phase += state->freq * 2.f / (float) SAMPLE_RATE;

        if (state->phase > 1.f)
            state->phase -= 2.f;
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Starting");

    wave_state_t *wave_state = malloc(sizeof(*wave_state));
    wave_state->phase = 0.f;
    wave_state->freq = 220.f;

    i2sbuf_config_t config = {
            .i2s_port = I2S_NUM,
            .ws_io = I2S_WS,
            .do_io = I2S_DO,
            .clk_io = I2S_CK,
            .sample_rate = SAMPLE_RATE,
            .use_apll = false,
            .buf_len = BUF_LEN,
            .buf_count = BUF_COUNT,
            .callback = audio_callback,
            .user_data = wave_state,
    };

    ESP_ERROR_CHECK(i2sbuf_install(&config));

    ESP_LOGI(TAG, "Playing");
}
