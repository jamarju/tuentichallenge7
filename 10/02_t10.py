#!/usr/bin/env python2.7

# This script assumes the precalculated secret1 and secret2 table output by 01_extract_secrets.sh
# in a file named 'secrets.txt'

import hashlib
import os

import binascii

secrets = {}
with open("secrets.txt") as f:
    for line in f:
        line = line.rstrip()
        (d, s1, s2) = line.split()
        s1 = int(s1)
        s2 = int(s2)
        if d in secrets:
            print "PANIC!!"
        else:
            secrets[d] = (s1, s2)

nc = int(raw_input())
for c in xrange(nc):    # test case
    md5 = None
    (u, nl) = raw_input().split()
    for l in xrange(int(nl)):   # date + nchanges
        (d, nch) = raw_input().split()
        (s1, s2) = secrets[d]
        for ch in xrange(int(nch)):     # same day changes
            if md5 is None:  # user password never set
                s3 = binascii.crc32(u) & 0xffffffff
            else:
                s3 = binascii.crc32(md5) & 0xffffffff

            counter = s3 * pow(s1, 10000000, s2) % s2
            pw = ""

            for i in xrange(10):
                counter = counter * s1 % s2
                pw += chr(counter % 94 + 33)

            m = hashlib.md5()
            m.update(pw)
            md5 = m.hexdigest()
    print "Case #" + str(c + 1) + ": " + pw
