import easydraw, network, esp32, system, keyboard, buttons, display, listbox, nvs

easydraw.messageCentered("Scanning...", True, "/media/wifi.png")
sta_if = network.WLAN(network.STA_IF); sta_if.active(True)
scanResults = sta_if.scan()

ssidList = []
for AP in scanResults:
	ssidList.append(AP[0])
ssidSet = set(ssidList)

options = listbox.List(0, 0, display.width(), display.height())

for ssid in ssidSet:
	try:
		ssidStr = ssid.decode("ascii")
		options.add_item(ssidStr)
	except:
		pass

chosenSsid = ""
def connectClick(pushed):
	global chosenSsid
	if pushed:
		selected = options.selected_text().encode()
		
		ssidType = scanResults[ssidList.index(selected)][4]
		if ssidType == 5:
			easydraw.messageCentered("WPA Enterprise is not supported yet.", True, "/media/alert.png")
			system.reboot()
		
		chosenSsid = selected
		if ssidType == 0:
			passInputDone(None)
		else:
			keyboard.show("Password","",passInputDone)

def passInputDone(password):
	global chosenSsid
	nvs.nvs_setstr("system", "wifi.ssid", chosenSsid)
	if password:
		nvs.nvs_setstr("system", "wifi.password", password)
	else:
		try:
			nvs.nvs_erase("system", "wifi.password")
		except:
			pass
	easydraw.messageCentered("Settings stored!", True, "/media/ok.png")
	system.launcher()

def exitApp(pressed):
	if pressed:
		system.launcher()

buttons.attach(buttons.BTN_A, connectClick)
buttons.attach(buttons.BTN_B, exitApp)
try:
	buttons.attach(buttons.BTN_START, exitApp)
except:
	pass
buttons.attach(buttons.BTN_UP, lambda pushed: display.flush() if pushed else 0)
buttons.attach(buttons.BTN_DOWN, lambda pushed: display.flush() if pushed else 0)
display.flush()
