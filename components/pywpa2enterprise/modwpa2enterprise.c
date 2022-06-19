#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "py/obj.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "mphalport.h"
#include "modmachine.h"
#include "wifi_connect.h"
#include "wifi_connection.h"

esp_err_t driver_wpa2enterprise_init() { return ESP_OK; }

//------------------------------------------------------------------------
STATIC mp_obj_t mod_wpa2enterprise_wifi_connect_to_stored () {
    return wifi_connect_to_stored() ? mp_const_true : mp_const_false;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_wpa2enterprise_wifi_connect_to_stored_obj, mod_wpa2enterprise_wifi_connect_to_stored);

STATIC mp_obj_t mod_wpa2enterprise_wifi_init () {
    wifi_init();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_wpa2enterprise_wifi_init_obj, mod_wpa2enterprise_wifi_init);

STATIC mp_obj_t mod_wpa2enterprise_wifi_disconnect () {
    wifi_disconnect();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_wpa2enterprise_wifi_disconnect_obj, mod_wpa2enterprise_wifi_disconnect);

// To be done
//bool wifi_connect(const char* aSsid, const char* aPassword, wifi_auth_mode_t aAuthmode, uint8_t aRetryMax);
//bool wifi_connect_ent(const char* aSsid, const char *aIdent, const char *aAnonIdent, const char* aPassword, esp_eap_ttls_phase2_types phase2, uint8_t aRetryMax);


//=========================================================
STATIC const mp_rom_map_elem_t wpa2enterprise_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect_to_stored), MP_ROM_PTR(&mod_wpa2enterprise_wifi_connect_to_stored_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&mod_wpa2enterprise_wifi_init_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&mod_wpa2enterprise_wifi_disconnect_obj) }
};
STATIC MP_DEFINE_CONST_DICT(wpa2enterprise_module_globals, wpa2enterprise_module_globals_table);

//===================================
const mp_obj_module_t wpa2enterprise_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&wpa2enterprise_module_globals,
};
