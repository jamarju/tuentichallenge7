nc = int(raw_input());

for case in xrange(nc):
    (s, c, d) = map(int, raw_input().split()) # 1, 2, 1
    #(s, c, d) = (0, 8, 0)

    uc = int(c / 2) * 2

    while True:
        if uc < 4:
            uc = 0
            us = 0
            ud = 0
            break

        if uc == 4:
            us = int(s / 2) * 2
            if s >= 2:
                ud = d
            else:
                ud = int(d / 2) * 2
            break

        if (uc - 2) % 4 == 0 and s >= 2:  # uc=6,10,14,... need 2s to close
            us = int(s / 2) * 2
            ud = d
            break

        if uc == 8 and (s >= 2 or d >= 1):  # 8 needs 2s or 1d to close
            us = int(s / 2) * 2
            ud = d
            break

        if uc >= 12 and uc % 4 == 0:
            us = int(s / 2) * 2
            ud = d
            break

        uc -= 2

    print "Case #" + str(case + 1) + ": " + str(uc + us + ud)
