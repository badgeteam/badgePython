import woezel, wifi, uinterface, system, gc, time
from default_icons import icon_no_wifi, animation_connecting_wifi
from esp32 import NVS

nvs = NVS("uinstaller")
nvs_launcher = NVS("launcher")

def install(app_name):
    nvs.set_blob('to_install', app_name)
    nvs.commit()
    system.start('uinstaller')


if system.__current_app__ == 'uinstaller':
    appname = bytearray(40)
    nvs.get_blob("to_install", appname)
    to_install = appname.decode("utf-8").rstrip('\x00')
    nvs.set_blob('to_install', "")
    nvs.commit()
    print('Installing %s' % to_install)
    if not to_install:
        system.reboot()

    ### For some reason normal uinterface.connect_wifi() doesn't
    ### work from dashboard.terminal.installer, so we have to do this
    ### odd workaround of connecting twice.
    wifi.connect()

    # rgb.clear()
    # data, size, frames = animation_connecting_wifi
    # rgb.framerate(3)
    # rgb.gif(data, (12, 0), size, frames)

    del data, size, frames, animation_connecting_wifi
    gc.collect()
    wifi.wait()
    wifi.connect()
    wifi.wait()

    if not wifi.status():
        data, frames = icon_no_wifi
        #rgb.gif(data, (12, 0), (8, 8), frames)
        time.sleep(3)
        system.reboot()
    ###

    # rgb.clear()
    # rgb.framerate(20)
    uinterface.loading_text('Installing %s' % to_install)
    del icon_no_wifi
    gc.collect()
    if woezel.install(to_install):
        # Reset launcher's selected index to newly installed app
        nvs_launcher.set_i32('index', 0)
        nvs_launcher.commit()
        #rgb.clear()
        uinterface.skippabletext('Install succeeded')
    else:
        #rgb.clear()
        uinterface.skippabletext('Failed to install "%s"' % to_install)
        
    system.reboot()