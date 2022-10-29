import tasks.otacheck as otacheck, term, system, time, wifi, machine, consts

current_build = int(consts.INFO_FIRMWARE_BUILD)
current_name = consts.INFO_FIRMWARE_NAME

term.header(True, "Update check")
print("Checking for updates...")
print("")
print("Currently installed:",current_name,"(Build "+str(current_build)+")")

available = 0

nvs = esp32.NVS("badge")
nvs.set_i32('OTA.ready', 0)
nvs.commit()

def start(pressed):
	if pressed:
		system.ota()

if not wifi.status():
	wifi.connect()
	if not wifi.wait():
		print("Error: could not connect to WiFi!")

title = "Update check"
message = "?? Unknown state ??"
		
if wifi.status():
	info = otacheck.download_info()
	if info:
			print("Server has: ",info['name']," (Build "+str(info['build'])+")")
			if info["build"] > current_build:
				nvs.set_i32('OTA.ready', 1)
				nvs.commit()
				print("Update available!")
				print(str(info["build"])+") "+info["name"])
				print("Update now?")
				title = "Firmware update available"
				message  = "A new firmware version is available. Update?\n"
				message += "Currently installed: "+current_name+" (Build "+str(current_build)+")\n"
				message += "Available          : "+info["name"]+" (Build "+str(info["build"])+")"
			elif info["build"] < current_build:
				print("Server has an older version.")
				nvs.set_i32('OTA.ready', 1)
				nvs.commit()
				print("Downgrade available!")
				print(str(info["build"])+") "+info["name"])
				print("Downgrade now?")
				title = "Firmware downgrade available"
				message  = "An older firmware version is available. Update?\n"
				message += "Currently installed: "+current_name+" (Build "+str(current_build)+")\n"
				message += "Available          : "+info["name"]+" (Build "+str(info["build"])+")"
			else:
				print("You are up-to-date!")
				print("Up-to-date!")
				print(str(current_build)+") "+current_name)
				print("Update anyway?")
				title = "Up-to-date"
				message = "You are up-to-date.\n"
				message += "Currently installed: "+current_name+" (Build "+str(current_build)+")\n"
				message += "Available          : "+info["name"]+" (Build "+str(info["build"])+")"
	else:
		print("An error occured!")
		print("Check failed.")
		print(str(current_build)+") "+current_name)
		print("Update anyway?")
		title = "Update check"
		message = "An error occured while fetching information. You can still choose to start the OTA procedure."
else:
	print("No WiFi!")
	print(str(current_build)+") "+current_name)
	print("Update anyway?")
	title = "Update check"
	message = "Could not connect to the WiFi network. You can still choose to start the OTA procedure."

items = ["Cancel", "Start OTA update"]
callbacks = [system.home, system.ota]
callbacks[term.menu(title, items, 0, message)]()
