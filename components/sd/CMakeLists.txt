idf_component_register(
    SRCS "sd.c"
    INCLUDE_DIRS "."
    REQUIRES driver sdmmc fatfs i2c gpio io_extension
)
