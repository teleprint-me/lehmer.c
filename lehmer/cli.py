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

    def select(self, position: int) -> None:
        self.position = position % self.length

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
    """Set up and parse command-line arguments for the Lehmer RNG generator."""
    parser = argparse.ArgumentParser(
        description="Lehmer RNG seed generator and sequence explorer.",
        epilog=(
            "Example Usage: "
            "`python -m lehmer.cli --seed 42 --length 128 --position 10 --normalize`\n"
            "Description: Generates a sequence of Lehmer RNG seeds with a given length, "
            "starting seed, and provides options to view normalized values."
        ),
    )

    # Core parameters
    parser.add_argument(
        "-z",
        "--seed",
        type=int,
        default=123456789,
        help="Initial seed value for the RNG. Default is 123456789.",
    )
    parser.add_argument(
        "-l",
        "--length",
        type=int,
        default=256,
        help="Number of seeds to generate in the sequence. Default is 256.",
    )
    parser.add_argument(
        "-p",
        "--position",
        type=int,
        default=0,
        help="Starting position in the sequence. Default is 0.",
    )
    parser.add_argument(
        "-m",
        "--modulus",
        type=int,
        default=2147483647,
        help=(
            "Modulus value for the Lehmer RNG (must be a Mersenne prime). "
            "Default is 2147483647 (2^31 - 1)."
        ),
    )
    parser.add_argument(
        "-a",
        "--multiplier",
        type=int,
        default=48271,
        help=(
            "Multiplier value for the Lehmer RNG. Must be a prime number less than the modulus. "
            "Default is 48271."
        ),
    )

    # Iteration and normalization control
    parser.add_argument(
        "-i",
        "--iterations",
        type=int,
        default=10000,
        help="Number of iterations to perform on the sequence. Default is 10000.",
    )
    parser.add_argument(
        "-n",
        "--normalize",
        action="store_true",
        help="If set, output the normalized value of the seed (float between 0.0 and 1.0).",
    )

    # Verbosity and output options
    parser.add_argument(
        "-v",
        "--verbose",
        action="store_true",
        help="If set, output all generated seeds in the sequence. Default is False.",
    )
    parser.add_argument(
        "-q",
        "--quiet",
        action="store_true",
        help="Suppress all non-essential output (overrides --verbose). Default is False.",
    )

    # Version and debugging options
    parser.add_argument(
        "--version",
        action="version",
        version="Lehmer RNG CLI v1.0",
        help="Show the version number and exit.",
    )
    return parser.parse_args()


def main():
    args = get_arguments()

    # Create an instance of LehmerState using the parsed arguments
    lehmer_rng = LehmerState(
        seed=args.seed,
        length=args.length,
    )

    # Print initial state if verbose is enabled
    if args.verbose and not args.quiet:
        print(f"Initial seed: {lehmer_rng.seed}")
        print(f"Sequence length: {lehmer_rng.length}")

    # Display the generated sequence
    for i in range(args.iterations):
        seed_value = lehmer_rng.get_current_seed()
        if args.normalize:
            normalized_value = lehmer_rng.normalize()
            output = f"Position: {i}, Seed: {seed_value}, Normalized: {normalized_value:.10f}"
        else:
            output = f"Position: {i}, Seed: {seed_value}"

        # Print each value if verbose, otherwise only the last value
        if args.verbose and not args.quiet:
            print(output)

        lehmer_rng.set_next_seed()
    else:
        seed_value = lehmer_rng.get_current_seed()

    # Final output for non-verbose mode
    if not args.verbose and not args.quiet:
        print(f"Final Seed: {seed_value}")


if __name__ == "__main__":
    main()
