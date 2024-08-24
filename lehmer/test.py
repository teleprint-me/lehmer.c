#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

Module: lehmer/test.py

NOTE: Test for correctness in implementation,
      not for randomness in output.
"""


from lehmer.generator import Lehmer

#
# simple tests
#


def test_seed_setting():
    rng = Lehmer(123456789)
    assert rng.z == 123456789


def test_generate_sequence():
    rng = Lehmer(123456789)
    sequence = [rng.y_random() for _ in range(10)]
    expected = [
        # NOTE: 0.053803 is a trusted magic value.
        # This magic value is only valid when:
        # m = 2**31 - 1, a = 48271, and s = 123456789
        0.05380315429242475,
        0.13206084963495882,
        0.7092727290975268,
        0.30390626671905924,
        0.8594007957072001,
        0.13580958225569204,
        0.6643450645098207,
        0.6006089535544669,
        0.9947970276674242,
        0.8473225342330162,
    ]
    assert all(abs(a - b) < 1e-9 for a, b in zip(sequence, expected))


def test_normalize():
    rng = Lehmer(123456789)
    rng.z = 123456789
    assert 0.0 <= rng.normalize() < 1.0


def test_full_period():
    count = 0
    seed = 1
    original_seed = seed
    rng = Lehmer(123456789)

    while seed is not original_seed and count < rng.m - 1:
        rng.z = rng.generate()  # generate a new seed every cycle
        count += 1

    assert count == (rng.m - 1)


#
# boundary tests
#


# fixture to abstract repeated code
def assert_lehmer_range(input: int, iterations: int) -> list[float]:
    rng = Lehmer(seed=input)
    sequence = [rng.y_random() for _ in range(iterations)]
    # Ensure values are in range and check if the sequence behaves as expected.
    assert all(0.0 <= value < 1.0 for value in sequence)


def test_seed_range():
    # For a seed of zero, the sequence should either remain zero or
    # behave in a defined way.
    assert_lehmer_range(0, 10_000)
    assert_lehmer_range(2**31 - 1, 10_000)
    assert_lehmer_range(1, 10_000)
    assert_lehmer_range(2**31 - 1 // 2, 10_000)
    # Optionally, check for specific expected values if they are known.


#
# distribution tests
#


def test_distribution():
    rng = Lehmer(123456789)
    sample_size = 10000
    sequence = [rng.y_random() for _ in range(sample_size)]

    mean = sum(sequence) / len(sequence)
    variance = sum((x - mean) ** 2 for x in sequence) / len(sequence)

    # Check if mean is close to 0.5 (expected for a uniform distribution)
    assert abs(mean - 0.5) < 0.01

    # Check if variance is close to 1/12 (variance of uniform distribution in [0,1))
    assert abs(variance - 1 / 12) < 0.01


def test_non_prime_multiplier():
    rng = Lehmer(123456789)
    rng.a = 48270  # Example non-prime multiplier
    sequence = [rng.y_random() for _ in range(10000)]
    mean = sum(sequence) / len(sequence)
    assert abs(mean - 0.5) < 0.01


if __name__ == "__main__":
    test_seed_setting()
    test_generate_sequence()
    test_normalize()
    test_seed_range()
    test_distribution()
    test_non_prime_multiplier()
    print("Silence is golden <3")
