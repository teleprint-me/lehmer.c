#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

Script: examples/lehmer.py

Brief: Super simple implementation for seed generation
       for the Lehmer RNG in pure Python from scratch.
"""

import argparse

m = 2147483647  # modulus
a = 48271  # multiplier
j = 22925  # jump


def lehmer_generate_modulo(z: int) -> int:
    """scale and then return the remainder"""
    return (a * z) % m


def lehmer_random_modulo(z: int) -> float:
    """normalize the seed as a ratio of the modulus"""
    return float(z) / float(m)


def get_arguments() -> argparse.Namespace:
    """set and parse the command-line arguments"""
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
        "-v",
        "--value",
        action="store_true",
        help="Generate the normalized modulo value. Default is False.",
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = get_arguments()

    seed = args.seed
    for _ in range(args.iterations):
        seed = lehmer_generate_modulo(seed)
    print("Seed:", seed)

    if args.value:
        print("Value:", lehmer_random_modulo(seed))
