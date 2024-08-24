#!/usr/bin/env python

"""
Copyright © 2024 Austin Berrio

Module: lehmer/test.py
"""

from lehmer.generator import Lehmer


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


if __name__ == "__main__":
    test_seed_setting()
    test_generate_sequence()
    test_normalize()
    print("Silence is golden <3")
