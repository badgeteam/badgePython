import uinterface

uinterface.skippabletext('Recovery mode: attach USB')
print('Recovery mode activated. You can debug your badge via Python shell, and reboot with system.reboot().')
__import__('shell')