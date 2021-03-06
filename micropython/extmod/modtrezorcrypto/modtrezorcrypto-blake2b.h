/*
 * Copyright (c) Pavol Rusnak, SatoshiLabs
 *
 * Licensed under TREZOR License
 * see LICENSE file for details
 */

#include "py/objstr.h"

#include "trezor-crypto/blake2b.h"

#define HASH_BLAKE2B_BLOCK_SIZE   BLAKE2B_BLOCKBYTES
#define HASH_BLAKE2B_DIGEST_SIZE  BLAKE2B_OUTBYTES

typedef struct _mp_obj_Blake2b_t {
    mp_obj_base_t base;
    BLAKE2B_CTX ctx;
} mp_obj_Blake2b_t;

STATIC mp_obj_t mod_TrezorCrypto_Blake2b_update(mp_obj_t self, mp_obj_t data);

/// def trezor.crypto.hashlib.blake2b(data: bytes=None, key: bytes=None) -> Blake2b:
///     '''
///     Creates a hash context object.
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Blake2b_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 0, 2, false);
    mp_obj_Blake2b_t *o = m_new_obj(mp_obj_Blake2b_t);
    o->base.type = type;
    // constructor called with key argument set
    if (n_args == 2) {
        mp_buffer_info_t key;
        mp_get_buffer_raise(args[1], &key, MP_BUFFER_READ);
        blake2b_InitKey(&(o->ctx), BLAKE2B_OUTBYTES, key.buf, key.len);
    } else {
        blake2b_Init(&(o->ctx), BLAKE2B_OUTBYTES);
    }
    // constructor called with data argument set
    if (n_args >= 1) {
        mod_TrezorCrypto_Blake2b_update(MP_OBJ_FROM_PTR(o), args[0]);
    }
    return MP_OBJ_FROM_PTR(o);
}

/// def trezor.crypto.hashlib.blake2b.update(self, data: bytes) -> None:
///     '''
///     Update the hash context with hashed data.
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Blake2b_update(mp_obj_t self, mp_obj_t data) {
    mp_obj_Blake2b_t *o = MP_OBJ_TO_PTR(self);
    mp_buffer_info_t msg;
    mp_get_buffer_raise(data, &msg, MP_BUFFER_READ);
    if (msg.len > 0) {
        blake2b_Update(&(o->ctx), msg.buf, msg.len);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_TrezorCrypto_Blake2b_update_obj, mod_TrezorCrypto_Blake2b_update);

/// def trezor.crypto.hashlib.blake2b.digest(self) -> bytes:
///     '''
///     Returns the digest of hashed data.
///     '''
STATIC mp_obj_t mod_TrezorCrypto_Blake2b_digest(mp_obj_t self) {
    mp_obj_Blake2b_t *o = MP_OBJ_TO_PTR(self);
    vstr_t vstr;
    vstr_init_len(&vstr, HASH_BLAKE2B_DIGEST_SIZE);
    BLAKE2B_CTX ctx;
    memcpy(&ctx, &(o->ctx), sizeof(BLAKE2B_CTX));
    blake2b_Final(&ctx, (uint8_t *)vstr.buf, BLAKE2B_OUTBYTES);
    memset(&ctx, 0, sizeof(BLAKE2B_CTX));
    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_TrezorCrypto_Blake2b_digest_obj, mod_TrezorCrypto_Blake2b_digest);

STATIC mp_obj_t mod_TrezorCrypto_Blake2b___del__(mp_obj_t self) {
    mp_obj_Blake2b_t *o = MP_OBJ_TO_PTR(self);
    memset(&(o->ctx), 0, sizeof(BLAKE2B_CTX));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_TrezorCrypto_Blake2b___del___obj, mod_TrezorCrypto_Blake2b___del__);

STATIC const mp_rom_map_elem_t mod_TrezorCrypto_Blake2b_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_update), MP_ROM_PTR(&mod_TrezorCrypto_Blake2b_update_obj) },
    { MP_ROM_QSTR(MP_QSTR_digest), MP_ROM_PTR(&mod_TrezorCrypto_Blake2b_digest_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&mod_TrezorCrypto_Blake2b___del___obj) },
    { MP_ROM_QSTR(MP_QSTR_block_size), MP_OBJ_NEW_SMALL_INT(HASH_BLAKE2B_BLOCK_SIZE) },
    { MP_ROM_QSTR(MP_QSTR_digest_size), MP_OBJ_NEW_SMALL_INT(HASH_BLAKE2B_DIGEST_SIZE) },
};
STATIC MP_DEFINE_CONST_DICT(mod_TrezorCrypto_Blake2b_locals_dict, mod_TrezorCrypto_Blake2b_locals_dict_table);

STATIC const mp_obj_type_t mod_TrezorCrypto_Blake2b_type = {
    { &mp_type_type },
    .name = MP_QSTR_Blake2b,
    .make_new = mod_TrezorCrypto_Blake2b_make_new,
    .locals_dict = (void*)&mod_TrezorCrypto_Blake2b_locals_dict,
};
