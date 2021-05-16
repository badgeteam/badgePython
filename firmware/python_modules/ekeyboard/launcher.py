import display
import platform
import os
import woezel
import sys
import machine
import system
import ujson
import esp32

nvs = esp32.NVS("launcher")

apps = []
current_index = 0
current_icon = None
enabled_keyboard = True

numlock = False
capslock = False

def add_app(app, information):
    global apps
    install_path = woezel.get_install_path()
    try:
        title = information["name"]
    except:
        title = app
    try:
        category = information["category"]
    except:
        category = ""
    try:
        if category == "system":
            icon = {'data': information["icon"]}
        else:
            icon = {'path': '%s/%s/icon.png' % (install_path, app)}

    except:
        icon = {'data': icon_unknown}

    info = {"file": app, "title": title, "category": category, "icon": icon}
    apps.append(info)

def populate_apps():
    global apps, current_index
    apps = []
    try:
        userApps = os.listdir('apps')
        userApps.reverse()
    except OSError:
        userApps = []
    for app in userApps:
        add_app(app, read_metadata(app))
    
    try:
        current_index = nvs.get_i32('index')
    except:
        current_index = 0
    if current_index >= len(apps):
        current_index = 0

# Read app metadata
def read_metadata(app):
    try:
        install_path = woezel.get_install_path()
        info_file = "%s/%s/metadata.json" % (install_path, app)
        with open(info_file) as f:
            information = f.read()
        return ujson.loads(information)
    except BaseException as e:
        #print("[ERROR] Can not read metadata for app " + app)
        sys.print_exception(e)
        information = {"name": app, "description": "", "category": "", "author": "", "revision": 0}
        return [app, ""]

def statushandler(led):
    global numlock, capslock
    numlock = led & 0x01
    capslock = led & 0x02
    render_app()

def keyhandler(state, key):
    global enabled_keyboard, apps, current_index
    if key == 0xFF and state == platform.PRESSED:
        enabled_keyboard = not enabled_keyboard
        platform.usb_hid(enabled_keyboard)
        render_app()
    if not enabled_keyboard:
        if key == 0x50 and state == platform.PRESSED:
            current_index = current_index - 1
            if current_index < 0:
                current_index = len(apps) - 1
            render_app()
        if key == 0x4F and state == platform.PRESSED:
            current_index = current_index + 1
            if current_index == len(apps):
                current_index = 0
            render_app()
        if key == 0x28 and state == platform.PRESSED:
            platform.usb_hid()
            system.start(apps[current_index]["file"], status=True)

def render_app():
    global current_index, apps, numlock, capslock, enabled_keyboard
    


    display.drawFill(0)
    
    display.drawLine(0, 50, 127, 50, 0xFFFFFF)
    display.drawLine(77, 1, 77, 48, 0xFFFFFF)
    
    if len(apps) > 0:
        app = apps[current_index]
        title = app['title']
        width = display.getTextWidth(title)
        height = display.getTextHeight(title)

        posx = int((77-width)/2)
        posy = int((50-height)/2)
        display.drawText(posx, posy, app['title'])
        if app['icon']['path']:
            filename = app['icon']['path']
            try:
                display.drawPng(79,1, filename)
            except:
                print("No icon for app")
        elif app['icon']['data']:
            display.drawPng(79, 1, app['icon']['data'])   

    if numlock:
        display.drawText(0, 51, "NUM")
    if capslock:
        display.drawText(63-int(display.getTextWidth("CAPS")/2), 51, "CAPS")
    if not enabled_keyboard:
        display.drawText(126-display.getTextWidth("APP"), 51, "APP")
    display.flush()


populate_apps()
platform.add_keyboard_handler(keyhandler)
platform.add_statusled_handler(statushandler)
render_app()