PORT ?= /dev/ttyACM0

IDF_PATH ?= $(shell cat .IDF_PATH 2>/dev/null || echo `pwd`/esp-idf)
IDF_TOOLS_PATH ?= $(shell cat .IDF_TOOLS_PATH 2>/dev/null || echo `pwd`/esp-idf-tools)
IDF_BRANCH ?= v5.5.1
IDF_EXPORT_QUIET ?= 1
IDF_GITHUB_ASSETS ?= dl.espressif.com/github_assets
MAKEFLAGS += --silent

BUILDDIR ?= build
IDF_EXPORT_QUIET ?= 0
SHELL := /usr/bin/env bash

.PHONY: prepare clean build flash erase monitor menuconfig image qemu install prepare-mch2022 prepare-cz19 mch2022 clean-frozen

all: prepare build

.PHONY: sdk
sdk:
	if test -d "$(IDF_PATH)"; then echo -e "ESP-IDF target folder exists!\r\nPlease remove the folder or un-set the environment variable."; exit 1; fi
	if test -d "$(IDF_TOOLS_PATH)"; then echo -e "ESP-IDF tools target folder exists!\r\nPlease remove the folder or un-set the environment variable."; exit 1; fi
	git clone --recursive --branch "$(IDF_BRANCH)" https://github.com/espressif/esp-idf.git "$(IDF_PATH)" --depth=1 --shallow-submodules
	cd "$(IDF_PATH)"; git submodule update --init --recursive
	cd "$(IDF_PATH)"; bash install.sh all

prepare: sdk
	git submodule update --init --recursive
	cd components/micropython/micropython/mpy-cross; make
	cp configs/default_defconfig sdkconfig
	cp partition_tables/default.csv partitions.csv

prepare-mch2022: prepare
	cp configs/mch2022_defconfig sdkconfig
	cp partition_tables/mch2022.csv partitions.csv
	
prepare-campzone2019: prepare
	cp configs/campzone2019_defconfig sdkconfig
	cp partition_tables/campzone2019.csv partitions.csv

prepare-sha2017: prepare
	cp configs/sha2017_defconfig sdkconfig
	cp partition_tables/sha2017.csv partitions.csv

clean:
	rm -rf "$(BUILDDIR)"
	rm components/micropython/mpconfigoverrides.h
	rm main/platform_gen.c
	source "$(IDF_PATH)/export.sh" && idf.py clean

build:
	source "$(IDF_PATH)/export.sh" && idf.py build

flash: build
	source "$(IDF_PATH)/export.sh" && idf.py flash -p $(PORT)

erase:
	source "$(IDF_PATH)/export.sh" && idf.py erase-flash -p $(PORT)

reconfigure:
	source "$(IDF_PATH)/export.sh" && idf.py reconfigure

monitor:
	source "$(IDF_PATH)/export.sh" && idf.py monitor -p $(PORT)

menuconfig:
	source "$(IDF_PATH)/export.sh" && idf.py menuconfig

install: flash

mch2022: prepare-mch2022 build

sha2017: prepare-sha2017 build

clean-frozen:
	rm -rf build/frozen_content.c
