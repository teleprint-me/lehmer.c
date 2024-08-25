#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

Script: examples/lehmer.py

Brief: Super simple implementation for seed generation for the Lehmer RNG in pure Python from scratch
"""

import argparse

m = 2147483647  # modulus
a = 48271  # multiplier
j = 22925  # jump


def lehmer_seed_modulo(z: int) -> int:
    return (a * z) % m


def get_arguments() -> argparse.Namespace:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-s",
        "--seed",
        default=123456789,
        help="The seed used as input. Default is 123456789.",
    )
    parser.add_argument(
        "-i",
        "--iterations",
        default=10000,
        help="Number of iterations before generating output. Default is 10000.",
    )
    parser.add_argument(
        "--modulo-value",
        action="store_true",
        help="Generate the modulo value instead of the seed. Default is False.",
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = get_arguments()

    seed = 123456789  # seed
    for _ in range(10_000):
        seed = lehmer_seed_modulo(seed)
    print(seed)
