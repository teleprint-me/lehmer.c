#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

Module: lehmer/generator.py

Simple proof-of-concept for the lehmer generator in pure python

lehmer generator : az mod m

a is the multiplier
z is the seed
m is the modulus

NOTE: Algorithm is the same for int and float
"""


class Lehmer:
    m = 2**31 - 1  # Mersenne prime
    a = 48271  # Multiplier

    def __init__(self, seed: int):
        self.z = seed

    @property
    def seed(self) -> int:
        return self.z

    @seed.setter
    def seed(self, value: int) -> None:
        self.z = abs(value) % self.m

    def generate(self) -> int:
        self.z = (self.a * self.z) % self.m
        return self.z

    def normalize(self) -> float:
        # Normalize to range [0.0, 1.0)
        return self.z / self.m

    def random(self) -> float:
        # Calculate new seed using the Lehmer formula
        q = self.m // self.a
        r = self.m % self.a
        new_seed = self.a * (self.z % q) - r * (self.z // q)

        # Ensure the seed remains positive
        if new_seed < 0:
            new_seed += self.m

        # Normalize to range [0.0, 1.0)
        return new_seed / self.m


if __name__ == "__main__":
    lehmer = Lehmer(seed=123456789)

    print("Generated stream:")
    output = lehmer.stream(10)
    for element in output:
        print(element)
    print()

    last = [lehmer.generate() for _ in range(10_000)][-1]
    print(f"Last generated value after 10,000 iterations: {last}")
