#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

Module: lehmer/primality.py

Pre-calculated Mersenne prime tables do exist.
https://www.mersenne.org/primes/

Miller-Rabin primality test may be helpful.
https://www.geeksforgeeks.org/primality-test-set-3-miller-rabin/

NOTE: Large prime values are both computationally complex and expensive.
      As a result, this module is kept as simple as possible simply to
      validate the selected values based on the paper.
"""

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


def is_prime(n):
    if n < 2:
        return False
    i = 2
    while i * i <= n:
        if n % i == 0:
            return False
        i += 1
    return True


def test_lehmer_primality() -> None:
    print("Testing Lehmer RNG parameters")
    for prime in TRUSTED_MULTIPLIERS:
        print(f"MULTIPLIER {prime} is prime? {is_prime(prime)}")
    for prime in TRUSTED_MODULI:
        print(f"MODULUS {prime} is prime? {is_prime(prime)}")


if __name__ == "__main__":
    test_lehmer_primality()
