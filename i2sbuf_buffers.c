#include "i2sbuf_buffers.h"
#include "i2sbuf.h"

QueueHandle_t generationQueue;

static int16_t buffer1[I2SBUF_BUF_SIZE];

i2sbuf_buffer_t buffer1_handle = {
		.buf = buffer1,
		.writeSemaphore = NULL
};
