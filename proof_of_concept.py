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

    def __init__(self, seed: int):
        self.seed = seed

    def lehmer_generator(self) -> int:
        self.seed = (self.MULTIPLIER * self.seed) % self.MODULUS
        return self.seed


if __name__ == '__main__':
    # Testing the generator
    lehmer = Lehmer(seed=123456789)
    output = [lehmer.lehmer_generator() for _ in range(10)]
    print(output)
