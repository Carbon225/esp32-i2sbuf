#ifndef COMPONENTS_I2SBUF_PRIVINCLUDE_I2S_INTERNAL_H_
#define COMPONENTS_I2SBUF_PRIVINCLUDE_I2S_INTERNAL_H_

#ifndef ERROR_CHECK_RETURN
#define ERROR_CHECK_RETURN(fn) ({                               \
	esp_err_t __ret = ESP_ERROR_CHECK_WITHOUT_ABORT((fn));      \
	if (__ret != ESP_OK) {                                      \
		return __ret;                                           \
	}                                                           \
	__ret;														\
})
#endif

#endif /* COMPONENTS_I2SBUF_PRIVINCLUDE_I2S_INTERNAL_H_ */
