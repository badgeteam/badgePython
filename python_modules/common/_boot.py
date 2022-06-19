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
	uos.mkdir('/cache')
except:
	pass
try:
	uos.mkdir('/config')
except:
	pass
sys.path.append(".frozen")
sys.path.append("/apps")
sys.path.append("/sd/apps")
gc.collect()
