#include <string.h>

#include "esp_system.h"
#include "esp_log.h"
#include "driver/spi_common.h"

#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "vfs.h"
#include "vfs_native.h"

STATIC mp_obj_t sdcard_mount(size_t n_args, const mp_obj_t *args)
{
	if (n_args > 0) {
		int chd = mp_obj_get_int(args[0]);
		if (chd) {
		    mount_vfs(VFS_NATIVE_TYPE_SDCARD, VFS_NATIVE_EXTERNAL_MP);
		}
	}
	else mount_vfs(VFS_NATIVE_TYPE_SDCARD, NULL);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sdcard_mount_obj, 0, 1, sdcard_mount);

STATIC mp_obj_t sdcard_umount(void)
{
    // umount external (sdcard) file system
	mp_obj_t sddir = mp_obj_new_str(VFS_NATIVE_EXTERNAL_MP, strlen(VFS_NATIVE_EXTERNAL_MP));
	mp_call_function_1(MP_OBJ_FROM_PTR(&mp_vfs_umount_obj), sddir);

	// Change directory to internal flash directory
	sddir = mp_obj_new_str(VFS_NATIVE_INTERNAL_MP, strlen(VFS_NATIVE_INTERNAL_MP));
	mp_call_function_1(MP_OBJ_FROM_PTR(&mp_vfs_chdir_obj), sddir);

	return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sdcard_umount_obj, sdcard_umount);

STATIC const mp_rom_map_elem_t sdcard_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_mountsd),			MP_ROM_PTR(&sdcard_mount_obj) },
    { MP_ROM_QSTR(MP_QSTR_umountsd),		MP_ROM_PTR(&sdcard_umount_obj) }
};

STATIC MP_DEFINE_CONST_DICT(sdcard_module_globals, sdcard_module_globals_table);

const mp_obj_module_t sdcard_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&sdcard_module_globals,
};