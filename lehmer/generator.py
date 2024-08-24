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
    """
    A Python implementation of the Lehmer Random Number Generator (RNG).

    Attributes:
        z (int): The current seed value.
    """

    def __init__(self, z: int):
        """
        Initialize the Lehmer generator with a seed.

        Args:
            z (int): The initial seed value.
        """
        self.z = z  # Seed

    @property
    def m(self) -> int:
        """
        Modulus used in the Lehmer RNG. It's a Mersenne prime (2^31 - 1).

        Returns:
            int: The modulus value.
        """
        return 2**31 - 1

    @property
    def a(self) -> int:
        """
        Multiplier used in the Lehmer RNG. A prime scalar (48271).

        Returns:
            int: The multiplier value.
        """
        return 48271

    def y(self) -> int:
        """
        Calculate gamma (γ) based on the Lehmer formula.

        γ(z) = a * (z % q) - r * (z // q)
        Where q = m // a and r = m % a

        Returns:
            int: The gamma value.
        """
        q = self.m // self.a
        r = self.m % self.a
        gamma = self.a * (self.z % q) - r * (self.z // q)
        return gamma if gamma >= 0 else gamma + self.m

    def d(self) -> int:
        """
        Calculate delta (δ) for potential analysis or adjustments.

        δ(z) = (z // q) - (a * z // m)

        Returns:
            int: The delta value.
        """
        q = self.m // self.a
        return (self.z // q) - (self.a * self.z // self.m)

    def normalize(self) -> float:
        """
        Normalize the current seed value to a float in the range [0.0, 1.0).

        Returns:
            float: The normalized value.
        """
        return self.z / self.m

    def scale(self) -> int:
        """
        Scale the seed using the multiplier, used in generating the next seed.

        Returns:
            int: The scaled seed value.
        """
        return abs(self.a * self.z)

    def generate(self) -> int:
        """
        Generate the next seed in the sequence using the Lehmer formula.

        z = (a * z) % m

        Returns:
            int: The next seed value.
        """
        return self.scale() % self.m

    def y_random(self) -> float:
        """
        Generate a pseudo-random float using the gamma (y) value.

        Returns:
            float: The normalized random value.
        """
        self.z = self.generate()
        self.seed = self.y()
        return self.normalize()

    def d_random(self) -> float:
        """
        Generate a pseudo-random float using the delta (d) value.

        Returns:
            float: The normalized random value.
        """
        self.z = self.generate()
        self.seed = self.y()
        return self.normalize()


# Example usage:
if __name__ == "__main__":
    # Initialize the Lehmer RNG with a seed
    rng = Lehmer(123456789)

    # Generate and print a series of pseudo-random numbers
    for _ in range(10):
        print(rng.y_random())
