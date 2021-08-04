#include "i2sbuf_task.h"
#include "i2sbuf.h"

void i2sbuf_task(void *args)
{
	int16_t buf[CONFIG_I2SBUF_DMA_BUF_LEN][2];

	i2sbuf_config_t config = *(i2sbuf_config_t*)args;
	free(args);

	for (;;)
	{
		config.callback(buf, CONFIG_I2SBUF_DMA_BUF_LEN, config.user_data);
		i2s_write(config.i2s_port, buf, sizeof buf, NULL, portMAX_DELAY);
	}
}
