import hashlib

DHASH_SIZE = 32
DHASH_MIN = "0" * DHASH_SIZE
DHASH_MAX = "f" * DHASH_SIZE

def dhash(data):
    h = hashlib.md5()
    h.update(data)
    return h.hexdigest()

def dhash_to_int(h):
    return int(h, 16)

def int_to_dhash(v):
    return "{0:0{1}x}".format(v, DHASH_SIZE)
