import ctypes, os


class DealUid(ctypes.Structure):
    _fields_ = [("i1", ctypes.c_longlong), ("i2", ctypes.c_longlong)]


wwdeal = ctypes.cdll.LoadLibrary("./libwwdeal.so")
wwdeal.ww_deal.restype = DealUid


# deal_uid_t ww_deal(const uint8_t key[32], uint64_t event_num, uint32_t session_num, uint32_t board_num);
def deal(key=None, board_num=1, event_num=1, session_num=1):
    if not key :
        key = os.urandom(32)

    while len(key) < 32 :
        key = key + key

    if len(key) > 32 :
        key = key[0:32]

    return wwdeal.ww_deal(
        ctypes.c_char_p(key),
        ctypes.c_ulonglong(event_num),
        ctypes.c_ulong(session_num),
        ctypes.c_ulong(board_num))


# void ww_deal_to_pbn(deal_uid_t deal_uid, char pbn_deal[58], unsigned board_num);
def to_pbn(deal_uid, board_num=1):
    pbn = b'\0' * 70
    wwdeal.ww_deal_to_pbn(deal_uid, ctypes.c_char_p(pbn), ctypes.c_ulong(board_num))
    return pbn.decode('ascii')


# void ww_deal_to_uuid(deal_uid_t deal_uid, char deal_uuid[25]);
def to_uuid(deal_uid):
    uuid = b'\0' * 25
    wwdeal.ww_deal_to_uuid(deal_uid, ctypes.c_char_p(uuid))
    return uuid.decode('ascii')

if __name__ == '__main__' :
    for i in range(1, 33) :
        d = deal(None, i)
        p = to_pbn(d)
        u = to_uuid(d)
        print(f"{i:02d} {u} {p}")

