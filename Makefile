.PHONY: vendor

JOBS=4
MAKE=make -j $(JOBS)

TREZORHAL_BUILD_DIR=micropython/trezorhal/build

TREZORHAL_PORT_OPTS=FROZEN_MPY_DIR=../../../src
UNIX_PORT_OPTS=MICROPY_FORCE_32BIT=1 MICROPY_PY_BTREE=0 MICROPY_PY_TERMIOS=0 MICROPY_PY_FFI=0 MICROPY_PY_USSL=0 MICROPY_SSL_AXTLS=0
CROSS_PORT_OPTS=MICROPY_FORCE_32BIT=1

help: ## show this help
	@awk 'BEGIN {FS = ":.*?## "} /^[a-zA-Z0-9_-]+:.*?## / {printf "\033[36mmake %-20s\033[0m %s\n", $$1, $$2}' $(MAKEFILE_LIST)

vendor: ## update git submodules
	git submodule update --init

res: ## update resources
	./tools/res_collect

build: build_trezorhal build_unix build_cross ## build trezorhal, unix and mpy-cross micropython ports

build_trezorhal: vendor res build_cross ## build trezorhal port with frozen modules
	$(MAKE) -C vendor/micropython/trezorhal $(TREZORHAL_PORT_OPTS)

build_trezorhal_debug: vendor res build_cross ## build trezorhal port with frozen modules and debug symbols
	$(MAKE) -C vendor/micropython/trezorhal $(TREZORHAL_PORT_OPTS) DEBUG=1

build_unix: vendor ## build unix port
	$(MAKE) -C vendor/micropython/unix $(UNIX_PORT_OPTS)

build_unix_debug: vendor ## build unix port with debug symbols
	$(MAKE) -C vendor/micropython/unix $(UNIX_PORT_OPTS) DEBUG=1

build_cross: vendor ## build mpy-cross port
	$(MAKE) -C vendor/micropython/mpy-cross $(CROSS_PORT_OPTS)

run: ## run unix port
	cd src ; ../vendor/micropython/unix/micropython

emu: ## run emulator
	./emu.sh

clean: clean_trezorhal clean_unix clean_cross ## clean all builds

clean_trezorhal: ## clean trezorhal build
	$(MAKE) -C vendor/micropython/trezorhal clean $(TREZORHAL_PORT_OPTS)

clean_unix: ## clean unix build
	$(MAKE) -C vendor/micropython/unix clean $(UNIX_PORT_OPTS)

clean_cross: ## clean mpy-cross build
	$(MAKE) -C vendor/micropython/mpy-cross clean $(CROSS_PORT_OPTS)

flash: ## flash firmware using st-flash
	st-flash write $(TREZORHAL_BUILD_DIR)/firmware.bin 0x8000000

flash_openocd: $(TREZORHAL_BUILD_DIR)/firmware.hex ## flash firmware using openocd
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg \
		-c "init" \
		-c "reset init" \
		-c "stm32f4x mass_erase 0" \
		-c "flash write_image $(TREZORHAL_BUILD_DIR)/firmware.hex" \
		-c "reset" \
		-c "shutdown"

openocd: ## start openocd which connects to the device
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg

gdb: ## start remote gdb session which connects to the openocd
	arm-none-eabi-gdb $(TREZORHAL_BUILD_DIR)/firmware.elf -ex 'target remote localhost:3333'

test: ## run unit tests
	cd tests ; ./run_tests.sh

testpy: ## run selected unit tests from python-trezor
	cd tests ; ./run_tests_python_trezor.sh
