#!/usr/bin/env python3
import re, math
from functools import reduce

hex_table = ['a', 'b', 'c', 'd', 'f']

def bin_to_dec(bin):
    assert(re.match("^[01]+$", bin))
    return reduce(lambda x, y: x + y, [(2**i) for i, x in enumerate(bin[::-1]) if x == '1'])

def hex_to_dec(hex):
    assert(re.match("^[0-9a-f]+$", hex, re.I))
    return reduce(lambda x, y: x + y, [int(hex_table.index(x) + 11 if x in hex_table else x) * (16 ** i) for i, x in enumerate(hex[::-1])])

def dec_to_hex(n):
    assert(isinstance(n, int) or n.isnumeric())
    return None

def dec_to_bin(n):
    assert(isinstance(n, int) or n.isnumeric())
    return None

def factoral(n):
    return reduce(lambda x, y: x * y, [z for z in range(n, 0, -1)])

def quadratic(a, b, c):
    return ((-b + math.sqrt(b ** 2 - 4 * a * c)) / 2 * a,
            (-b - math.sqrt(b ** 2 - 4 * a * c)) / 2 * a)


print(factoral(5))
print(bin_to_dec("01101110"))
print(dec_to_bin(110))
print(hex_to_dec("3f48"))
print(dec_to_hex(16200))
