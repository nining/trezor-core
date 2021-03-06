# Automatically generated by pb2py
import protobuf as p
from .CoinType import CoinType

class Features(p.MessageType):
    FIELDS = {
        1: ('vendor', p.UnicodeType, 0),
        2: ('major_version', p.UVarintType, 0),
        3: ('minor_version', p.UVarintType, 0),
        4: ('patch_version', p.UVarintType, 0),
        5: ('bootloader_mode', p.BoolType, 0),
        6: ('device_id', p.UnicodeType, 0),
        7: ('pin_protection', p.BoolType, 0),
        8: ('passphrase_protection', p.BoolType, 0),
        9: ('language', p.UnicodeType, 0),
        10: ('label', p.UnicodeType, 0),
        11: ('coins', CoinType, p.FLAG_REPEATED),
        12: ('initialized', p.BoolType, 0),
        13: ('revision', p.BytesType, 0),
        14: ('bootloader_hash', p.BytesType, 0),
        15: ('imported', p.BoolType, 0),
        16: ('pin_cached', p.BoolType, 0),
        17: ('passphrase_cached', p.BoolType, 0),
        18: ('firmware_present', p.BoolType, 0),
    }
    MESSAGE_WIRE_TYPE = 17
