# BadgePython
Experimental try at making an ESP-IDF v4.0 compatible Badge oriented Micropython firmware

## Supported environments
* Linux
* Mac OS
* (Maybe WSL 2 on Windows?)

## Setup (generic, default configuration)
1. Clone the repo
2. Run `make prepare` to clone submodules and install the configuration files
3. Run `make build` to build the firmware
4. Run `make flash` to flash the firmware to the device

## Setup (MCH2022 badge app)
1. Clone the repo
2. Run `make prepare-mch2022` to clone submodules and install the configuration files
3. Run `make build` to build the firmware
4. Install the file `build/badge_firmware.bin` as an app via WebUSB

## Other make commands
* `make clean` remove existing build files
* `make erase` completely wipe clean the flash memory of the target device
* `make flash` program the bootloader, partition table and firmware into the target device
* `make monitor` to open the serial monitor and debugger application
* `make menuconfig` to open the SDK menuconfig configuration interface

## Changes over old firmware
1. python module bridges (e.g. modi2c.c) are now placed inside the driver folder. So the micropython folder stays untouched. You need to register the file for it to be include in the build. More on this later.  
2. Init in platform.c are now also handled with a generator so you only need to register the init function with cmake. When properly done no dummy init functions are necessary when the driver is disabled.  
3. All drivers are now compiled with cmake.  
4. uPy is now based on upstream micropython.    
5. Bluetooth support is enabled (BLE only for now).    

## Registering a new driver
A working example can be found in de driver_bus_i2c folder.  
In CMakelists.txt added all sources necessary for your driver and requirement for micropython and any other module.  
If there is configuration option to disable the driver wrap you srcs in an cmake if statement.  
To register the micropython bindings append your files/name as follows:
 >set(EXTMODS "${EXTMODS}" "${mods}" CACHE INTERNAL "")  
 >set(EXTMODS_NAMES "${EXTMODS_NAMES}" "${mod_name}" CACHE INTERNAL "")  
 
 where mods is all the full filepaths containing micropython bindings and mod_name contains the module name
 in the module files all includes not from uPy should be wrapped in #ifndef NO_QSTR 
To register driver with an init function register as followed:  
 >set(EXTMODS_INIT "${EXTMODS_INIT}" "\"${mod_name}\"@\"${mod_register}\"^" CACHE INTERNAL "")  
 
 where mod_register is the register message. This can also be done in a cmake if statement to not register the init/module when the driver is disabled.  
 Registering should be done in the project_include.cmake file. This ensures that the mods are registered before micropython is build.

## Registering builtin python modules
In the main folder there are 2 folders for this. Manifests and python_modules. The python_modules folder is collection of modules people can include in their badge build. To include one these modules add to your badge specific manifest. This manifest should be stored in the manifests folder and can be selected in menuconfig.  
The $(PORT_DIR) var is linked to the modules folder. The $(BOARD_DIR) is in python_modules/badge_hardware_name folder. This is included in the default manifest. The $(MPY_DIR) var is linked to the uPy component folder. DO NOT store modules in here.


## Issues
1. QSTR regeneration may or may not be supported
2. uPy configuration options need to be readded.
3. Expect a lot of bugs ;)
