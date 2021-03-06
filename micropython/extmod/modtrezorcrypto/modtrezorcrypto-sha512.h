/*
 * Copyright (c) Pavol Rusnak, SatoshiLabs
 *
 * Licensed under TREZOR License
 * see LICENSE file for details
 */

#include "py/objstr.h"

#include "trezor-crypto/sha2.h"

#define HASH_SHA512_BLOCK_SIZE   SHA512_BLOCK_LENGTH
#define HASH_SHA512_DIGEST_SIZE  SHA512_DIGEST_LENGTH

typedef struct _mp_obj_Sha512_t {
    mp_obj_base_t base;
    SHA512_CTX ctx;
} mp_obj_Sha512_t;

STATIC mp_obj_t mod_TrezorCrypto_Sha512_update(mp_obj_t self, mp_obj_t data);

/// def trezor.crypto.hashlib.sha512(data: bytes=None) -> Sha512:
///     '''
///     Creates a hash context object.
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Sha512_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 1, false);
    mp_obj_Sha512_t *o = m_new_obj(mp_obj_Sha512_t);
    o->base.type = type;
    sha512_Init(&(o->ctx));
    if (n_args == 1) {
        mod_TrezorCrypto_Sha512_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

/// def trezor.crypto.hashlib.sha512.hash(self, data: bytes) -> None:
///     '''
///     Update the hash context with hashed data.
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Sha512_update(mp_obj_t self, mp_obj_t data) {
    mp_obj_Sha512_t *o = MP_OBJ_TO_PTR(self);
    mp_buffer_info_t msg;
    mp_get_buffer_raise(data, &msg, MP_BUFFER_READ);
    if (msg.len > 0) {
        sha512_Update(&(o->ctx), msg.buf, msg.len);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_TrezorCrypto_Sha512_update_obj, mod_TrezorCrypto_Sha512_update);

/// def trezor.crypto.hashlib.sha512.digest(self) -> bytes:
///     '''
///     Returns the digest of hashed data.
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Sha512_digest(mp_obj_t self) {
    mp_obj_Sha512_t *o = MP_OBJ_TO_PTR(self);
    vstr_t vstr;
    vstr_init_len(&vstr, HASH_SHA512_DIGEST_SIZE);
    SHA512_CTX ctx;
    memcpy(&ctx, &(o->ctx), sizeof(SHA512_CTX));
    sha512_Final(&ctx, (uint8_t *)vstr.buf);
    memset(&ctx, 0, sizeof(SHA512_CTX));
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_TrezorCrypto_Sha512_digest_obj, mod_TrezorCrypto_Sha512_digest);

STATIC mp_obj_t mod_TrezorCrypto_Sha512___del__(mp_obj_t self) {
    mp_obj_Sha512_t *o = MP_OBJ_TO_PTR(self);
    memset(&(o->ctx), 0, sizeof(SHA512_CTX));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_TrezorCrypto_Sha512___del___obj, mod_TrezorCrypto_Sha512___del__);

STATIC const mp_rom_map_elem_t mod_TrezorCrypto_Sha512_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&mod_TrezorCrypto_Sha512_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&mod_TrezorCrypto_Sha512_digest_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mod_TrezorCrypto_Sha512___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_block_size), MP_OBJ_NEW_SMALL_INT(HASH_SHA512_BLOCK_SIZE) },
    { MP_ROM_QSTR(MP_QSTR_digest_size), MP_OBJ_NEW_SMALL_INT(HASH_SHA512_DIGEST_SIZE) },
};
STATIC MP_DEFINE_CONST_DICT(mod_TrezorCrypto_Sha512_locals_dict, mod_TrezorCrypto_Sha512_locals_dict_table);

STATIC const mp_obj_type_t mod_TrezorCrypto_Sha512_type = {
    { &mp_type_type },
    .name = MP_QSTR_Sha512,
    .make_new = mod_TrezorCrypto_Sha512_make_new,
    .locals_dict = (void*)&mod_TrezorCrypto_Sha512_locals_dict,
};
