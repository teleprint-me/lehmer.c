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
        # Set the seed
        self.seed = seed
        # Set the initial position in the sequence
        self.position = 0
        # Coerce a size of 1 if size is 0
        self.length = 1 if 0 == size else size
        # Initialize the seed list with correct size and set the first seed
        self.sequence = [seed % self.MODULUS] * self.length
        if self.sequence[self.position] < 0:
            self.sequence[self.position] += self.MODULUS

        # Initialize remaining streams based on the first one
        for i in range(1, self.length):
            z = self.sequence[i - 1]  # Previous seed
            self.sequence[i] = (self.MULTIPLIER * z) % self.MODULUS
            if self.sequence[i] < 0:  # Handle any negative seeds
                self.sequence[i] += self.MODULUS

    @property
    def current_seed(self) -> int:
        return self.sequence[self.position]

    @current_seed.setter
    def current_seed(self, value: int) -> None:
        self.sequence[self.position] = value % self.MODULUS

    def select(self, stream: int) -> None:
        self.position = stream % self.length

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
        "-r",
        "--stream",
        type=int,
        default=0,
        help="The selected stream. Default is 0.",
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
    lehmer = LehmerState(
        size=args.size,
        seed=args.seed,
    )

    lehmer.select(args.stream)

    for i in range(args.iterations):
        seed = lehmer.modulo()

        if args.verbose:
            print(
                f"Iteration {i + 1}: "
                f"stream = {lehmer.stream}, seed = {seed}"
            )

    print(f"Initial seed: {lehmer.seed}")
    print(f"After {args.iterations} iterations:")
    print(f"Position {lehmer.position} final seed: {lehmer.sequence[0]}")

    if args.normalize:
        normalized_value = lehmer.normalize()
        print(
            f"Normalized Position {lehmer.position} value: {normalized_value}"
        )


if __name__ == "__main__":
    main()
