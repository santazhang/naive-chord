from dht.hashing import DHASH_MIN, DHASH_MAX, dhash_to_int, int_to_dhash

class keyrange(object):

    def __init__(self, low=DHASH_MIN, high=DHASH_MAX):
        self.low = low
        self.high = high

    def __contains__(self, item):
        return self.low <= item and item <= self.high

    def __str__(self):
        return "%s~%s" % (self.low, self.high)

    def __repr__(self):
        return self.__str__()

    def __eq__(self, other):
        return self.low == other.low and self.high == other.high

    def __hash__(self):
        return self.low.__hash__() ^ self.high.__hash__()

    def size(self):
        lv = dhash_to_int(self.low)
        hv = dhash_to_int(self.high)
        return hv - lv + 1

    def split(self, n=2):
        assert n >= 1
        if n == 1:
            return [self]
        lv = dhash_to_int(self.low)
        stride = self.size()
        each = stride // n
        if each == 0:
            return [keyrange(int_to_dhash(lv + i), int_to_dhash(lv + i)) for i in range(stride)]
        else:
            sp = []
            pos = lv
            for i in range(n - 1):
                rng = keyrange(int_to_dhash(pos), int_to_dhash(pos + each - 1))
                pos += each
                sp += rng,
            sp += keyrange(int_to_dhash(pos), self.high),
            return sp

    def include(self, other):
        return self.low <= other.low and other.high <= self.high