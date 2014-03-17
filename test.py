#!/usr/bin/env python

import unittest
from dht.hashing import dhash, DHASH_SIZE, DHASH_MIN, DHASH_MAX, dhash_to_int, int_to_dhash
from dht.keyrange import keyrange

class TestDHT(unittest.TestCase):

    def test_hashing(self):
        v = dhash("x")
        print v
        print DHASH_SIZE, DHASH_MIN, DHASH_MAX
        u = dhash_to_int(v)
        print u
        print int_to_dhash(u)
        assert int_to_dhash(dhash_to_int(v)) == v
        assert dhash_to_int(int_to_dhash(u)) == u

    def test_keyrange(self):
        print keyrange()
        assert dhash("x") in keyrange()
        tbl = {}
        ra = keyrange(int_to_dhash(0), int_to_dhash(99))
        rb = keyrange(int_to_dhash(100), int_to_dhash(199))
        tbl[keyrange(int_to_dhash(0), int_to_dhash(99))] = "svr_addr1"
        tbl[keyrange(int_to_dhash(100), int_to_dhash(199))] = "svr_addr2"
        assert tbl[ra] == "svr_addr1"
        assert tbl[rb] == "svr_addr2"
        print ra.split(1)
        print ra.split()
        print ra.split(7)
        assert len(ra.split(1000)) == ra.size()

if __name__ == "__main__":
    unittest.main()
