#!/usr/bin/env python3

"""
Copyright © 2024 Austin Berrio

Script: examples/lehmer.py

Brief: Super simple implementation for seed generation
       for the Lehmer RNG in pure Python from scratch.
"""

import argparse

MODULUS = 2147483647
MULTIPLIER = 48271


def lehmer_generate_modulo(seed: int) -> int:
    """Scale the seed and return the remainder."""
    return (MULTIPLIER * seed) % MODULUS


# NOTE: Normalization "generates" the pseudo random number
def lehmer_seed_normalize(seed: int) -> float:
    """Normalize the seed as a ratio of the modulus."""
    return seed / MODULUS


def get_arguments() -> argparse.Namespace:
    """Set and parse the command-line arguments."""
    parser = argparse.ArgumentParser(description="Lehmer RNG seed generator.")
    parser.add_argument(
        "-s",
        "--seed",
        type=int,
        default=123456789,
        help="The initial seed value. Default is 123456789.",
    )
    parser.add_argument(
        "-i",
        "--iterations",
        type=int,
        default=10000,
        help="Number of iterations. Default is 10000.",
    )
    parser.add_argument(
        "-n",
        "--normalize",
        action="store_true",
        help="Output the normalized value. Default is False.",
    )
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="Output all generated seeds. Default is False.",
    )
    return parser.parse_args()


if __name__ == "__main__":
    args = get_arguments()

    seed = args.seed
    for i in range(args.iterations):
        seed = lehmer_generate_modulo(seed)
        if args.verbose:
            print(f"Iteration {i + 1}: seed = {seed}")

    last_iteration = args.iterations
    print(f"After {last_iteration} iterations: seed = {seed}")

    if args.normalize:
        normalized_value = lehmer_seed_normalize(seed)
        print(f"Normalized value: {normalized_value}")
