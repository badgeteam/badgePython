#!/usr/bin/env bash

cd esp-idf
./install.sh
cd ../firmware/components/micropython/micropython/mpy-cross
make
