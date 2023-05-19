PORT ?= /dev/ttyACM0
BUILDDIR ?= build
BOARD ?= default
IDF_PATH ?= $(shell pwd)/esp-idf
IDF_EXPORT_QUIET ?= 0
SHELL := /usr/bin/env bash
MPYCROSS_SUBDIR := $(CURDIR)/components/micropython/micropython/mpy-cross
MPYCROSS_FILES := $(MPYCROSS_SUBDIR)/build/main.o $(MPYCROSS_SUBDIR)/mpy-cross


.PHONY: mpy_cross prepare clean build flash erase monitor menuconfig image qemu install clean-frozen git-submodule-reset

all: prepare build

PARTITION_TABLE_FILE := $(if $(wildcard partition_tables/$(BOARD).csv),partition_tables/$(BOARD).csv,partition_tables/default.csv)

partitions.csv: ${PARTITION_TABLE_FILE}
	cp ${PARTITION_TABLE_FILE} partitions.csv

sdkconfig: configs/$(BOARD)_defconfig
	cp configs/$(BOARD)_defconfig sdkconfig

$(MPYCROSS_FILES): mpy_cross;

mpy_cross:
	$(MAKE) -C $(MPYCROSS_SUBDIR)

prepare: partitions.csv sdkconfig $(MPYCROSS_FILES)
	git submodule update --init --recursive
	cd esp-idf; bash install.sh; cd ..

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

clean-frozen:
	rm -rf build/frozen_content.c

git-submodule-reset:
	git submodule foreach --recursive git clean -ffdx
	git submodule update --init --recursive
	git submodule foreach --recursive git reset --hard --recurse-submodules
