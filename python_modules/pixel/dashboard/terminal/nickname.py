import network, term, sys, system, machine
import esp32

nvs = esp32.NVS("badge")

system.serialWarning()


def home():
	system.home(True)


def main():
	term.empty_lines()
	term.header(True, "Nickname setup")
	print("Enter your new nickname.")
	nickname = term.prompt("Nickname", 1,5)
	if (len(nickname) >= 1):
		confirm(nickname)

def confirm(nickname):
	term.header(True, "Nickname setup")
	nvs.set_blob("nickname", nickname)
	nvs.commit()
	print("New configuration has been saved.")
	print("")
	print("Nickname:\t\t"+nickname)
	print("Press any key to return to the homescreen")
	sys.stdin.read(1)
	system.home(True)

while True:
	main()
