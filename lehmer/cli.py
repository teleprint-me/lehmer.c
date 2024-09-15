#!/usr/bin/env python3

"""
Copyright © 2024 Austin Berrio

Script: lehmer/cli.py

Brief: Super simple implementation for seed generation
       for the Lehmer RNG in pure Python from scratch.
"""

import argparse


class LehmerState:
    MODULUS = 2147483647
    MULTIPLIER = 48271

    def __init__(self, size: int, value: int):
        self.seed = []
        self.stream = 0
        self.size = 0

    @property
    def seed(self) -> int:
        return self.seed[self.stream]

    @seed.setter
    def seed(self, value: int):
        self.seed[self.stream] = value % self.MODULUS

    def select(self, stream: int):
        self.stream = stream % self.size

    # NOTE: Modulo "generates" the pseudo random seed
    def modulo(self) -> int:
        """Scale the seed and return the remainder."""
        seed = self.seed[self.stream]
        return (self.MULTIPLIER * seed) % self.MODULUS

    # NOTE: Normalization "generates" the pseudo random number
    def normalize(self) -> float:
        """Normalize the seed as a ratio of the modulus."""
        return self.seed[self.stream] / self.MODULUS


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
