#!/usr/bin/env python2.7

import sys


class Interval:
    def __init__(self, tup):
        self.a = tup[0]
        self.b = tup[1]
        self.children = []
        self.gap = tup[0]
        self.nimber = None
        self.followers = dict()

    def add_child(self, interval):
        self.children.append(interval)
        if interval.a == self.gap:  # assumes children are always inserted in ascending order!
            self.gap = interval.b + 1

    def is_parent_of(self, other):
        return self.a <= other.a and self.b >= other.b

    def calculate_nimber(self):
        """ Calculate the grundy number G aka nimber of this node
         
        Rules:

        - If this is a leaf interval, followers F = {0}. Followers can be understood as different game states for 
        this pile, with 0 = losing position.
        
        - If it's an inner interval, let Fij = jth follower of child i, Gi = grundy number of child i
        
            F = {F11,F1n} ^ G2 ^ G3 ^ ... ^ Gn U {F21,F2n} ^ G1 ^ G3 ^ ... ^ Gn U ... U {Fm1,Fmn} ^ G1 ^ G2 ^ ... ^ Gm-1
            
            Where {F11,F1n} are all the followers of child 1, and so on.
            
            By precalculating X = G1^G2^...^Gm, the latter can be somewhat simplified to:
            
            F = {F11,F1n} ^ X ^ G1 U {F21,F2n} ^ X ^ G2 U ... U {Fm1,Fmn} ^ X ^ Gm
            
        - If there isn't gap among children, F stays like that (the parent interval has no effect on the game 
        whatsoever)

        - If there is a gap among children, add the state resulting from picking the gap (ie. removing just the parent 
        interval):
        
            F = F U { X }
        
        Then, G = mex(F)
        
        :return: the nimber of this node 
        """
        if not self.children:
            self.followers[0] = self.a
        else:
            the_xor = reduce(lambda x, c: x ^ c.calculate_nimber(), self.children, 0)

            for c in self.children:
                for f in c.followers:
                    nf = f ^ the_xor ^ c.nimber
                    if nf not in self.followers:
                        self.followers[nf] = c.followers[f]

            if self.gap > self.b:  # no gap, easy peasy
                pass
            else:  # yes gap, OUCH! tricky
                self.followers[the_xor] = self.gap

        self.nimber = mex(self.followers)
        return self.nimber


def mex(s):
    i = 0
    while i in s:
        i += 1
    return i


def interval_cmp(x, y):
    """ Sort by a asc, then b desc
    """
    return y[1] - x[1] if x[0] == y[0] else x[0] - y[0]


def arrange_in_piles(board, piles):
    """ Generates all piles from the given board

    :param board: the list of intervals as tuples in preorder
    :param piles: empty array of piles (to fill out) 
    """

    parent_stack = []
    for i in xrange(len(board)):
        current = Interval(board[i])

        # Pop parents until we find one to adopt the child
        while parent_stack and not parent_stack[-1].is_parent_of(current):
            parent_stack.pop()

        if parent_stack:
            parent_stack[-1].add_child(current)
        else:  # it's a root
            piles.append(current)

        parent_stack.append(current)


def main(argv):

    testcase = None

    if len(argv) <= 1:
        sys.stderr.write(argv[0] + ": missing testfile\n")
        exit(1)
    else:
        testfile = argv[1]
    if (len(argv) >= 3):
        testcase = int(argv[2])

    with open(testfile) as fin:
        nc = int(fin.readline())

        for c in xrange(nc):

            # Parse board
            board = []
            piles = []
            ni = int(fin.readline())
            for i in xrange(ni):
                a, b = map(int, fin.readline().split())
                board.append((a, b))

            # Skip now if single testcase
            if testcase is not None and c + 1 != testcase:
                continue

            # Sort the intervals by a asc, b desc => preorder traversal of nesting tree, siblings in ascending order
            board.sort(cmp = interval_cmp)

            # Arrange intervals in piles
            arrange_in_piles(board, piles)

            # Calculate each pile's grundy number
            gg = reduce(lambda g, p: g ^ p.calculate_nimber(), piles, 0)

            print "Case #" + str(c + 1) + ":",
            if gg == 0:
                print "IMPOSSIBLE"
            else:
                # Find the first move that makes the overall game grundy number GG == 0.
                # We need to find a follower F in a pile with grundy number G such that GG ^ F = G
                # so that, if we replace the pile with grundy number G with F, the new game grundy number
                # NGG = GG ^ G ^ F = GG ^ GG ^ F ^ F = 0
                for p in piles:
                    f = gg ^ p.nimber
                    if f in p.followers:
                        print p.followers[f]
                        break

                #print "G=" + str(gg) + " F=" + str(p.followers)

            # Done if single testcase mode
            if testcase is not None:
                break

if __name__ == "__main__":
    main(sys.argv)
