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
    def __init__(self, z: int):
        """Initialize the Lehmer generator with a seed."""
        self.z = z  # Seed

    @property
    def m(self) -> int:
        return 2**31 - 1  # Modulus (a Mersenne prime)

    @property
    def a(self) -> int:
        return 48271  # Multiplier (a prime scalar)

    def y(self) -> int:
        """
        Calculate gamma (γ) based on the Lehmer formula:
        γ(z) = a * (z % q) - r * (z // q)
        Where q = m // a and r = m % a
        """
        q = self.m // self.a
        r = self.m % self.a
        return self.a * (self.z % q) - r * (self.z // q)

    def d(self) -> int:
        """
        Calculate delta (δ) as a potential additional component:
        δ(z) = (z // q) - (a * z // m)
        This can be used for further analysis or adjustments.
        """
        q = self.m // self.a
        return (self.z // q) - (self.a * self.z // self.m)

    def normalize(self) -> float:
        """
        Normalize the current seed value to a float in the range [0.0, 1.0).
        This is useful for generating pseudo-random floating-point numbers.
        """
        return self.z / self.m

    def scale(self) -> int:
        return abs(self.a * self.z)

    def generate(self) -> int:
        """
        Generate the next seed in the sequence using the standard Lehmer formula:
        z = (a * z) % m
        """
        return self.scale() % self.m

    def y_random(self) -> float:
        """
        Generate a pseudo-random floating-point number in the range [0.0, 1.0).
        Uses the gamma value (y) for this purpose.
        """
        # generate the next seed
        self.z = self.generate()
        # bind the generated seed
        self.seed = self.y()
        # normalize the generated seed
        return self.normalize()

    def d_random(self) -> float:
        # generate the next seed
        self.z = self.generate()
        # bind the generated seed
        self.seed = self.y()
        # normalize the generated seed
        return self.normalize()


if __name__ == "__main__":
    # Example usage:
    lehmer = Lehmer(seed=123456789)

    print("Generated stream:")
    output = [lehmer.random() for _ in range(10)]
    for element in output:
        print(element)
    print()

    last = [lehmer.generate() for _ in range(10_000)][-1]
    print(f"Last generated value after 10,000 iterations: {last}")
