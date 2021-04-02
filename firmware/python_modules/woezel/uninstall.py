import uos as os, time, machine, system
import uinterface, woezel, rgb
from esp32 import NVS

nvs = NVS("launcher")

app_bytes = bytearray(50)
app_file_bytes = bytearray(50)

nvs.get_blob("uninstall_name", app_bytes)
nvs.get_blob("uninstall_file", app_file_bytes)

app = app_bytes.decode("utf-8").rstrip('\x00')
app_file = app_file_bytes.decode("utf-8").rstrip('\x00')

if (not app) or not (app_file):
    system.home()

agreed = uinterface.confirmation_dialog('Uninstall \'%s\'?' % app)
if not agreed:
    system.home()

uinterface.loading_text("Removing " + app + "...")
install_path = woezel.get_install_path()
print(app)
print(app_file)
print(install_path)
for rm_file in os.listdir("%s/%s" % (install_path, app_file)):
    os.remove("%s/%s/%s" % (install_path, app_file, rm_file))
os.rmdir("%s/%s" % (install_path, app_file))

nvs.set_blob('uninstall_name', '')
nvs.set_blob('uninstall_file', '')
nvs.commit()

rgb.clear()
uinterface.skippabletext("Uninstall completed!")
system.home()
