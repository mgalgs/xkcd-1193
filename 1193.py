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

import sys
import random
import struct
import string
from skein import skein1024

def hash2bytes(thehash):
    return struct.unpack('128B', bytearray.fromhex(thehash))

THRESHOLD=421                   # from http://almamater.xkcd.com/best.csv (search for usu.edu)
target = '5b4da95f5fa08280fc9879df44f418c8f9f12ba424b7757de02bbdfbae0d4c4fdf9317c80cc5fe04c6429073466cf29706b8c25999ddd2f6540d4475cc977b87f4757be023f19b8f4035d7722886b78869826de916a79cf9c94cc79cd4347d24b567aa3e2390a573a373a48a5e676640c79cc70197e1c5e7f902fb53ca1858b6'
target_bytes = hash2bytes(target)

def sendit(what, offby):
    print ()
    print ('FOUND ONE!!!!!!!')
    print ('GO TRY ' + what)
    print ('(should be off by %d)' % offby)
    print ()

def getbit(b, bitnum):
    return (b & (1 << bitnum)) >> bitnum


print()

nattempts = 0
nuploads = 0
best_hash = 100000

while True:
    attempt_len = random.choice(range(50))
    attempt = ''.join(random.choice(string.ascii_uppercase + string.digits) for x in range(attempt_len))
    attempt_hash = skein1024(bytes(attempt, 'ascii'), digest_bits=1024).hexdigest()
    attempt_bytes = hash2bytes(attempt_hash)
    nbits = 0
    # see how many bits are different
    for attempt_byte, target_byte in zip(attempt_bytes, target_bytes):
        lbits = 0
        for b in range(8):
            if getbit(attempt_byte, b) != getbit(target_byte, b):
                lbits += 1
                nbits += 1
    best_hash = min(best_hash, nbits)
    if not nattempts % 1000:
        sys.stdout.write('\r%d hashes compared far (best so far is %d). %d uploaded...   ' % (
            nattempts, best_hash, nuploads))
    nattempts += 1
    if nbits < THRESHOLD:
        sendit(attempt, nbits)
        nuploads += 1
