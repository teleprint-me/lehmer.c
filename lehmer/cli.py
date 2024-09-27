#!/usr/bin/env python3

"""
Copyright © 2024 Austin Berrio

@file lehmer/cli.py

@brief CLI tool for validating the Lehmer RNG in pure Python from scratch.
"""

import argparse


class LehmerState:
    """Simplified Lehmer RNG state class for sequence generation and validation."""

    MODULUS = 2147483647  # Mersenne prime (2^31 - 1)
    MULTIPLIER = 48271  # Chosen multiplier (prime)

    def __init__(self, seed: int, length: int):
        """Mimic the C implementation for consistency and validity"""
        # Initialize state variables
        self.seed = seed % self.MODULUS  # Set initial seed
        self.position = 0  # Position in the sequence
        # Set length (coerce length to at least 1)
        self.length = max(1, length)
        # Allocate space for sequence storage
        self.sequence = [0] * self.length

        # Generate the initial sequence
        self.generate_sequence()

    def generate_sequence(self) -> None:
        """Generate a Lehmer sequence based on the initial seed."""
        # Generate the initial seed utilizing root seed
        self.sequence[0] = (self.MULTIPLIER * self.seed) % self.MODULUS
        # Generate the rest of the sequence
        for i in range(1, self.length):
            self.sequence[i] = (
                self.MULTIPLIER * self.sequence[i - 1]
            ) % self.MODULUS

    def set_initial_seed(self, seed: int) -> None:
        """Set the initial seed and regenerate the sequence."""
        self.seed = seed % self.MODULUS  # Set new initial seed
        self.position = 0  # Reset position
        self.generate_sequence()  # Regenerate the sequence

    def get_initial_seed(self) -> int:
        """Return the initial seed."""
        return self.seed

    def set_previous_seed(self) -> None:
        """Set the position to the previous seed in the sequence."""
        self.position = (self.position - 1) % self.length

    def set_next_seed(self) -> None:
        """Set the position to the next seed in the sequence."""
        self.position = (self.position + 1) % self.length

    def get_current_seed(self) -> int:
        """Get the current seed in the sequence."""
        self.position %= self.length  # Enforce position boundaries
        return self.sequence[self.position]

    def get_next_seed(self) -> int:
        """Move to the next seed and return the new current seed."""
        self.set_next_seed()
        return self.get_current_seed()

    def normalize(self) -> float:
        """Normalize the current seed to a float in the range 0.0 to 1.0."""
        return self.get_current_seed() / self.MODULUS

    def modulo(self) -> int:
        """Generate a new pseudo-random seed using the current state."""
        next_seed = (self.MULTIPLIER * self.get_current_seed()) % self.MODULUS
        # Update seed at the current position
        self.sequence[self.position] = next_seed
        return next_seed


def get_arguments() -> argparse.Namespace:
    """Set and parse the command-line arguments."""
    parser = argparse.ArgumentParser(description="Lehmer RNG seed generator.")
    parser.add_argument(
        "-z",
        "--seed",
        type=int,
        default=123456789,
        help="The initial seed value. Default is 123456789.",
    )
    parser.add_argument(
        "-l",
        "--length",
        type=int,
        default=256,
        help="The number of seeds. Default is 256.",
    )
    parser.add_argument(
        "-p",
        "--position",
        type=int,
        default=0,
        help="The selected position. Default is 0.",
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
        seed=args.seed,
        length=args.length,
    )

    lehmer.select(args.position)

    for i in range(args.iterations):
        seed = lehmer.modulo()

        if args.verbose:
            print(
                f"Iteration {i + 1}: "
                f"position = {lehmer.position}, seed = {seed}"
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
