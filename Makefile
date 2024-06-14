PORT ?= /dev/ttyACM0
BUILDDIR ?= build
IDF_PATH ?= $(shell pwd)/esp-idf
IDF_EXPORT_QUIET ?= 0
SHELL := /usr/bin/env bash

.PHONY: prepare clean build flash erase monitor menuconfig image qemu install prepare-mch2022 prepare-cz19 mch2022 clean-frozen

all: prepare build

prepare:
	git submodule update --init --recursive
	cd esp-idf; bash install.sh; cd ..
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
	source "$(IDF_PATH)/export.sh" && idf.py clean

build:
	source "$(IDF_PATH)/export.sh" && idf.py build

flash: build
	source "$(IDF_PATH)/export.sh" && idf.py flash -p $(PORT)

erase:
	source "$(IDF_PATH)/export.sh" && idf.py erase-flash -p $(PORT)

monitor:
	source "$(IDF_PATH)/export.sh" && idf.py monitor -p $(PORT)

menuconfig:
	source "$(IDF_PATH)/export.sh" && idf.py menuconfig

install: flash

mch2022: prepare-mch2022 build

sha2017: prepare-sha2017 build

clean-frozen:
	rm -rf build/frozen_content.c
