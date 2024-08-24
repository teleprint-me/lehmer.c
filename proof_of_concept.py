#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

lehmer generator

az mod m

a is the multiplier
z is the seed
m is the modulus

algorithm is the same for int and float
"""

class Lehmer:
    MODULUS = 2**63-1  # Must be a large prime integer
    MULTIPLIER = 48271
    A256 = 22925

    def lehmer_generator(self, seed: int) -> int:
        pass


if __name__ == '__main__':
    lehmer = Lehmer()
