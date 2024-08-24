#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

lehmer generator

az mod m

a is the multiplier
z is the seed
m is the modulus

algorithm is the same for int and float

NOTE: Large prime values are computationally expensive.

Pre-calculated Mersenne prime tables do exist.
https://www.mersenne.org/primes/

Miller-Rabin primality test may be helpful.
https://www.geeksforgeeks.org/primality-test-set-3-miller-rabin/
"""

import argparse


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
    MODULUS = 2**31 - 1  # Must be a Mersenne prime integer
    MULTIPLIER = 48271
    A256 = 22925

    def __init__(self, seed: int):
        self.seed = seed

    def lehmer_generator(self) -> int:
        self.seed = (self.MULTIPLIER * self.seed) % self.MODULUS
        return self.seed


def get_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Minimal standard implementation for the lehmer lcg"
    )
    parser.add_argument(
        "-p",
        "--primality",
        action="store_true",
        help="Enable primality testing. Slow for large prime numbers.",
    )
    return parser.parse_args()


# Trusted Prime Multiplier Numbers
TRUSTED_MULTIPLIERS = [
    16807,
    48271,
]

# Trusted Mersenne Prime Numbers
TRUSTED_MODULI = [
    2**31 - 1,
    2**61 - 1,
]


def test_common_parameters() -> None:
    print("Testing Lehmer RNG parameters")
    for prime in TRUSTED_MULTIPLIERS:
        print(f"MULTIPLIER {prime} is prime? {is_prime(prime)}")
    for prime in TRUSTED_MODULI:
        print(f"MODULUS {prime} is prime? {is_prime(prime)}")


if __name__ == "__main__":
    args = get_arguments()

    lehmer = Lehmer(seed=123456789)

    if args.primality:
        test_common_parameters()

    output = [lehmer.lehmer_generator() for _ in range(10)]
    for element in output:
        print(element)
