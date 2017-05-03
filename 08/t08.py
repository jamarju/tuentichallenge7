#!/usr/bin/python3

# Needs python3 because of unicode

import io
import codecs
import sys
import re


nre = re.compile('^\s*\d+\s*$')
c = -1

input_stream = io.TextIOWrapper(sys.stdin.buffer, encoding='utf-16', errors='strict')

line = input_stream.readline()
while line != "":

	c += 1
	if c == 0:
		line = input_stream.readline()
		continue

	print ("Case #" + str(c) + ": ", end="")

	if nre.search(line) is None:
		print ("N/A")
	else:
		print ("{:x}".format(int(line)))

	line = input_stream.readline()
