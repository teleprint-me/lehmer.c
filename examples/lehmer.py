#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

Script: examples/lehmer.py

Brief: Super simple implementation for seed generation for the Lehmer RNG in pure Python from scratch
"""

m = 2147483647  # modulus
a = 48271  # multiplier
j = 22925  # jump


def lehmer_seed_modulo(z: int) -> int:
    return (a * z) % m


if __name__ == "__main__":
    seed = 123456789  # seed
    for _ in range(10_000):
        seed = lehmer_seed_modulo(seed)
    print(seed)
