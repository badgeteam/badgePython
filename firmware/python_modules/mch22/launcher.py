import system, display, valuestore, wifi
import os, term_menu, time, machine
import hardware

menu = term_menu.UartMenu()
menu.main()

hardware.mountsd()
