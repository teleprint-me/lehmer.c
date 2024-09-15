#!/usr/bin/env python3

"""
Copyright © 2024 Austin Berrio

@file lehmer/cli.py

@brief CLI tool for validating the Lehmer RNG in pure Python from scratch.
"""

import argparse


class LehmerState:
    MODULUS = 2147483647
    MULTIPLIER = 48271

    def __init__(self, size: int, seed: int):
        """
        Mimic the C implementation for consistency and validity
        """
        # Select the first stream
        self.stream = 0
        # Coerce a size of 1 if size is 0
        self.size = 1 if 0 == size else size
        # Initialize the seed list with correct size and set the first seed
        self.seed = [seed % self.MODULUS] * self.size
        if self.seed[self.stream] < 0:
            self.seed[self.stream] += self.MODULUS

        # Initialize remaining streams based on the first one
        for i in range(1, self.size):
            z = self.seed[i - 1]  # Previous seed
            self.seed[i] = (self.MULTIPLIER * z) % self.MODULUS
            if self.seed[i] < 0:  # Handle any negative seeds
                self.seed[i] += self.MODULUS

    @property
    def current_seed(self) -> int:
        return self.seed[self.stream]

    @current_seed.setter
    def current_seed(self, value: int) -> None:
        self.seed[self.stream] = value % self.MODULUS

    def select(self, stream: int) -> None:
        self.stream = stream % self.size

    def modulo(self) -> int:
        """Generate a new pseudo random seed."""
        self.current_seed = (
            self.MULTIPLIER * self.current_seed
        ) % self.MODULUS
        return self.current_seed

    def normalize(self) -> float:
        """Normalize the seed as a ratio of the modulus."""
        return self.current_seed / self.MODULUS


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
        "-z",
        "--size",
        type=int,
        default=256,
        help="The number of streams. Default is 256.",
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


def main():
    args = get_arguments()

    # Initialize LehmerState
    lehmer = LehmerState(size=args.size, seed=args.seed)

    for i in range(args.iterations):
        lehmer.select(0)  # Stream 0
        seed_0 = lehmer.modulo()

        lehmer.select(1)  # Stream 1
        seed_1 = lehmer.modulo()

        if args.verbose:
            print(
                f"Iteration {i + 1}: "
                f"[stream = 0, seed = {seed_0}], "
                f"[stream = 1, seed = {seed_1}]"
            )

    print(f"After {args.iterations} iterations:")
    print(f"Stream 0 final seed: {lehmer.seed[0]}")
    print(f"Stream 1 final seed: {lehmer.seed[1]}")

    if args.normalize:
        normalized_value_0 = lehmer.normalize()
        lehmer.select(1)
        normalized_value_1 = lehmer.normalize()
        print(f"Normalized Stream 0 value: {normalized_value_0}")
        print(f"Normalized Stream 1 value: {normalized_value_1}")


if __name__ == "__main__":
    main()
