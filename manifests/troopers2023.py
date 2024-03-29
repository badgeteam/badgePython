freeze("../python_modules/troopers2023")
freeze("$(MPY_DIR)/tools", ("upip.py", "upip_utarfile.py"))
freeze("$(MPY_DIR)/drivers/dht", "dht.py")
freeze("$(MPY_DIR)/drivers/onewire")
freeze("$(MPY_DIR)/drivers/neopixel")
include("$(MPY_DIR)/extmod/uasyncio/manifest.py")
include("$(MPY_DIR)/extmod/webrepl/manifest.py")
freeze("../python_modules/generics", ("buttons.py"))
freeze("../python_modules/common", ("interface.py", "rtc.py", "upysh.py", "valuestore.py", "shell.py", "term.py", "virtualtimers.py", "system.py", "ntp.py", "_boot.py", "audio.py"))
freeze("../python_modules/common", "tasks/powermanagement.py")
freeze("../python_modules/common", "tasks/otacheck.py")
freeze("../python_modules/common", "umqtt")
freeze("../python_modules/woezel")
freeze("../python_modules/common", "dashboard/_installer_exec.py")
freeze("../python_modules/common", "dashboard/resources/woezel_repo.py")
freeze("../python_modules/core")
