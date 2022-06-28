import uos, gc, sys

try:
	uos.mkdir('/lib')
except:
	pass
try:
	uos.mkdir('/apps')
except:
	pass
try:
	uos.mkdir('/apps/python')
except:
	pass
try:
	uos.mkdir('/cache')
except:
	pass
try:
	uos.mkdir('/config')
except:
	pass

sys.path.append(".frozen")
sys.path.append("/apps/python")
sys.path.append("/sd/apps/python")
sys.path.append("/apps") # For backwards compatibility
sys.path.append("/sd/apps") # For backwards compatibility
gc.collect()
