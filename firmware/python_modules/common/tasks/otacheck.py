import wifi, time, consts
import esp32

nvs = NVS("system")

def download_info(show=False):
	import urequests as req
	result = False
	try:
		data = req.get(consts.OTA_WEB_PROTOCOL+"://"+consts.OTA_WEB_SERVER+":"+consts.OTA_WEB_PORT+consts.OTA_WEB_VERSION_PATH)
	except BaseException as e:
		print("Exception", e)
		return False
	try:
		result = data.json()
	except:
		data.close()
		return False
	data.close()
	return result

def available(update=False, show=False):
	if update:
		if not wifi.status():
			return nvs.get_i32('OTA.ready') == 1
		info = download_info(show)
		if info:
			if info["build"] > consts.INFO_FIRMWARE_BUILD:
				nvs.set_i32('OTA.ready', 1)
				nvs.commit()
				return True

		nvs.set_i32('OTA.ready', 0)
		nvs.commit()
	return nvs.get_i32('OTA.ready') == 1
