deps_config := \
	/home/jmeckst/esp/esp-idf/components/app_trace/Kconfig \
	/home/jmeckst/esp/esp-idf/components/aws_iot/Kconfig \
	/home/jmeckst/esp/esp-idf/components/bt/Kconfig \
	/home/jmeckst/esp/esp-idf/components/driver/Kconfig \
	/home/jmeckst/esp/esp-idf/components/esp32/Kconfig \
	/home/jmeckst/esp/esp-idf/components/esp_adc_cal/Kconfig \
	/home/jmeckst/esp/esp-idf/components/esp_event/Kconfig \
	/home/jmeckst/esp/esp-idf/components/esp_http_client/Kconfig \
	/home/jmeckst/esp/esp-idf/components/esp_http_server/Kconfig \
	/home/jmeckst/esp/esp-idf/components/ethernet/Kconfig \
	/home/jmeckst/esp/esp-idf/components/fatfs/Kconfig \
	/home/jmeckst/esp/esp-idf/components/freemodbus/Kconfig \
	/home/jmeckst/esp/esp-idf/components/freertos/Kconfig \
	/home/jmeckst/esp/esp-idf/components/heap/Kconfig \
	/home/jmeckst/esp/esp-idf/components/libsodium/Kconfig \
	/home/jmeckst/esp/esp-idf/components/log/Kconfig \
	/home/jmeckst/esp/esp-idf/components/lwip/Kconfig \
	/home/jmeckst/esp/esp-idf/components/mbedtls/Kconfig \
	/home/jmeckst/esp/esp-idf/components/mdns/Kconfig \
	/home/jmeckst/esp/esp-idf/components/mqtt/Kconfig \
	/home/jmeckst/esp/esp-idf/components/nvs_flash/Kconfig \
	/home/jmeckst/esp/esp-idf/components/openssl/Kconfig \
	/home/jmeckst/esp/esp-idf/components/pthread/Kconfig \
	/home/jmeckst/esp/esp-idf/components/spi_flash/Kconfig \
	/home/jmeckst/esp/esp-idf/components/spiffs/Kconfig \
	/home/jmeckst/esp/esp-idf/components/tcpip_adapter/Kconfig \
	/home/jmeckst/esp/esp-idf/components/unity/Kconfig \
	/home/jmeckst/esp/esp-idf/components/vfs/Kconfig \
	/home/jmeckst/esp/esp-idf/components/wear_levelling/Kconfig \
	/home/jmeckst/esp/esp-idf/components/app_update/Kconfig.projbuild \
	/home/jmeckst/esp/esp-idf/components/bootloader/Kconfig.projbuild \
	/home/jmeckst/esp/esp-idf/components/esptool_py/Kconfig.projbuild \
	/home/jmeckst/esp/BnoMaster/main/Kconfig.projbuild \
	/home/jmeckst/esp/esp-idf/components/partition_table/Kconfig.projbuild \
	/home/jmeckst/esp/esp-idf/Kconfig

include/config/auto.conf: \
	$(deps_config)

ifneq "$(IDF_TARGET)" "esp32"
include/config/auto.conf: FORCE
endif
ifneq "$(IDF_CMAKE)" "n"
include/config/auto.conf: FORCE
endif

$(deps_config): ;
