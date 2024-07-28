idf_component_register(
    SRCS i2sbuf.c i2sbuf_task.c
    INCLUDE_DIRS include
    PRIV_INCLUDE_DIRS PrivInclude
    REQUIRES driver
)
