#!/usr/bin/env python3

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
    print ('New best hash found!')
    print ('---BEGIN STRING:---')
    print ('%s' % what)
    print ('---END STRING---')
    print ('(should be off by %d)' % offby)
    print ()


def hashdiff(hash_Y_bytes,hash_Z_bytes):
    nbits = 0
    # see how many bits are different
    for Y, Z in zip(hash_Y_bytes, hash_Z_bytes):
        X = Y ^ Z
        while(X):
            X &= X - 1
            nbits += 1
    return nbits

def newstring(strlen,candidate_chars):
    theString = ''.join(random.choice(candidate_chars) for x in range(strlen))
    return theString


print()

best_hash = 1024
min_hash = ''
nattempts = 0
n = 0
candidate_chars = string.printable[:-6]
strlen = 2
attempt = newstring(strlen,candidate_chars)

while True:
    attempt_hash = skein1024(bytes(attempt, 'ascii'), digest_bits=1024).hexdigest()
    attempt_bytes = hash2bytes(attempt_hash)
    d = hashdiff(attempt_bytes,target_bytes)
    if d < best_hash:
        best_hash = d
        min_hash = attempt
        if d < THRESHOLD:
            sendit(attempt,d)
            THRESHOLD = d
    nattempts += 1
    n += 1
    if (n >> 10):
        sys.stdout.write('\r%d hashes compared. Best so far is %d' % (nattempts, best_hash))
        attempt = newstring(strlen-1,candidate_chars)
        n = 0
    attempt = attempt + random.choice(candidate_chars)

