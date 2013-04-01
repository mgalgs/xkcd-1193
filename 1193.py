#!/usr/bin/env python

# REQUIRES:
# pyskein http://pythonhosted.org/pyskein/
# python > 3.0 (due to pyskein)
#
# INSTALLATION:
# (install virtualenv)
# (install python3)
# virtualenv -p /path/to/python3 skein
# source skein/bin/activate
# pip install pyskein

import random
import struct
import string
from skein import skein1024

def hash2bytes(thehash):
    return struct.unpack('128B', bytearray.fromhex(thehash))

target = '5b4da95f5fa08280fc9879df44f418c8f9f12ba424b7757de02bbdfbae0d4c4fdf9317c80cc5fe04c6429073466cf29706b8c25999ddd2f6540d4475cc977b87f4757be023f19b8f4035d7722886b78869826de916a79cf9c94cc79cd4347d24b567aa3e2390a573a373a48a5e676640c79cc70197e1c5e7f902fb53ca1858b6'
target_bytes = hash2bytes(target)

# 0c41172b30c7c7bbc4291ce8e649b3b971a669bd6a0bb411de3076b3fe112a503e34e28292fb6093d82364d9da7a6ba793e42216da7c3e6afc30700cb6f27179485cedb0b5035ebe4fa9eedc2223ae2da28551b4553cec1cdfff62baa52111144163062ccae573ddf60b24de39be4f6088f3d2347882fdd3f3c4d059364ac302

def sendit(what, bit_error):
    print ('Trying ' + what)

while True:
    attempt_len = random.choice(range(50))
    attempt = ''.join(random.choice(string.ascii_uppercase + string.digits) for x in range(attempt_len))
    attempt = 'pizza'
    attempt_hash = skein1024(bytes(attempt, 'ascii'), digest_bits=1024).hexdigest()
    attempt_bytes = hash2bytes(attempt_hash)
    nbits = 0
    # see how many bits are different
    for attempt_byte, target_byte in zip(attempt_bytes, target_bytes):
        mesh = attempt_byte & target_byte
        lbits = 0
        for b in range(8):
            if not mesh & (1 << b):
                lbits += 1
                nbits += 1
        print ('comparing %s to %s %s (%d bits difff)' % (bin(attempt_byte), bin(target_byte), bin(mesh), lbits))
    print ('%s %s has %d bits different' % (attempt, attempt_hash, nbits))
    break
