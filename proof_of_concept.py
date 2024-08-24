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


def is_prime(n):
    if n < 2:
        return False
    i = 2
    while i * i <= n:
        if n % i == 0:
            return False
        i += 1
    return True


class Lehmer:
    MODULUS = 2**63 - 1  # Must be a large prime integer
    MULTIPLIER = 48271
    A256 = 22925

    def __init__(self, seed: int):
        self.seed = seed

    def lehmer_generator(self) -> int:
        self.seed = (self.MULTIPLIER * self.seed) % self.MODULUS
        return self.seed


if __name__ == "__main__":
    print(f"2^31-1 is prime? {is_prime(2**31-1)}")
    print(f"2^63-1 is prime? {is_prime(2**63-1)}")  # Corrected typo here

    # Testing the generator
    lehmer = Lehmer(seed=123456789)
    print(f"MODULUS is prime? {is_prime(lehmer.MODULUS)}")
    print(f"MULTIPLIER is prime? {is_prime(lehmer.MULTIPLIER)}")

    output = [lehmer.lehmer_generator() for _ in range(10)]
    for element in output:
        print(element)
