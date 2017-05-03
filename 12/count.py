#!/usr/bin/env python2.7

# Count coins using OpenCV template matching algorithm.

import numpy as np
import sys
import cv2

def search(img, template, output, color):
    w, h = template.shape[::-1]
    res = cv2.matchTemplate(img, template, cv2.TM_CCOEFF_NORMED)
    threshold = 0.9
    loc = np.where(res >= threshold)

    l = zip(*loc[::-1])
    if debug == 1:
        print str(l)
    for pt in l:
        cv2.rectangle(output, pt, (pt[0] + w, pt[1] + h), color, 2)

    return len(l)


filename = sys.argv[1]
debug = 0
if (len(sys.argv) > 2):
    debug = 1

t1 = cv2.imread("templates/1.png", 0)
t2 = cv2.imread("templates/2.png", 0)
t5 = cv2.imread("templates/5.png", 0)
t10 = cv2.imread("templates/10.png", 0)
t20 = cv2.imread("templates/20.png", 0)
t50 = cv2.imread("templates/50.png", 0)
t100 = cv2.imread("templates/100.png", 0)
t200 = cv2.imread("templates/200.png", 0)


image = cv2.imread(filename)
output = image.copy()
gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

t = 0 # total money

t += search(gray, t1, output, (255, 0, 0))
t += 2 * search(gray, t2, output, (0, 255, 0))
t += 5 * search(gray, t5, output, (0, 0, 255))
t += 10 * search(gray, t10, output, (255, 255, 0))
t += 20 * search(gray, t20, output, (255, 0, 255))
t += 50 * search(gray, t50, output, (0, 255, 255))
t += 100 * search(gray, t100, output, (128, 128, 128))
t += 200 * search(gray, t200, output, (128, 32, 0))

print str(t)
if debug == 1:
    cv2.imshow("output", np.hstack([image, output]))
    cv2.waitKey(0)

