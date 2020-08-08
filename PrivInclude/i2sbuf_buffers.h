#ifndef COMPONENTS_I2SBUF_PRIVINCLUDE_I2SBUF_BUFFERS_H_
#define COMPONENTS_I2SBUF_PRIVINCLUDE_I2SBUF_BUFFERS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

typedef struct {
	int16_t *buf;
	SemaphoreHandle_t writeSemaphore;
} i2sbuf_buffer_t;

extern QueueHandle_t generationQueue;
extern i2sbuf_buffer_t buffer1_handle;

#endif /* COMPONENTS_I2SBUF_PRIVINCLUDE_I2SBUF_BUFFERS_H_ */
