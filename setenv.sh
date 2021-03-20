#!/usr/bin/env bash

. ./esp-idf/export.sh
echo "Copy this into your IDE build env if it parses CMakeLists.txt (e.g. CLion needs this): IDF_PATH=${IDF_PATH};IDF_TOOLS_INSTALL_CMD=${IDF_TOOLS_INSTALL_CMD};IDF_PYTHON_ENV_PATH=${IDF_PYTHON_ENV_PATH};IDF_TOOLS_EXPORT_CMD=${IDF_TOOLS_EXPORT_CMD};OPENOCD_SCRIPTS=${OPENOCD_SCRIPTS};PATH=${PATH}"