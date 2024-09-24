/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/binder.c
 *
 * - **Bounded Results**: The equation
 * \( f(z) = \gamma(z) + m \cdot \delta(z) \)
 * ensures that all intermediate results are bounded by \( m - 1 \).
 *
 * Here:
 *   - $\gamma(z) = a \cdot (z \mod q) - r \cdot (z \div q)$
 *   - $\delta(z) = (z \div q) - (a \cdot z \div m)$
 * - **Significance of Constants**:
 *   - $\delta(z)$ is either 0 or 1.
 *   - Both $a \cdot (z \mod q)$ and $r \cdot (z \div q)$ are within the range
 *     $0, \dots, m-1$.
 *   - $\lvert \gamma(z) \rvert \leq m-1$.
 */

#include <stdint.h>
#include <stdio.h>

/**
 * @param LEHMER_MODULUS Mersenne prime number used as modulus (2^31 - 1).
 */
#define LEHMER_MODULUS 2147483647

/**
 * @param LEHMER_MULTIPLIER Prime number used to scale the seed.
 */
#define LEHMER_MULTIPLIER 48271

/**
 * @param LEHMER_QUOTIENT Range normalization
 */
#define LEHMER_QUOTIENT ((LEHMER_MODULUS) / (LEHMER_MULTIPLIER))

/**
 * @param LEHMER_REMAINDER Handle overflow and underflow
 */
#define LEHMER_REMAINDER ((LEHMER_MODULUS) % (LEHMER_MULTIPLIER))

/**
 * @param MAX_ELEMENTS Maximum number of elements to generate
 */
#define MAX_ELEMENTS 10

int32_t normalize_int(int32_t z, uint32_t m) {
    return (int32_t) z % (uint32_t) m;
}

float normalize_float(int32_t z, uint32_t m) {
    return (float) z / (float) m;
}

// using int32_t mitigates overflow and underflow
int32_t lehmer_rng(int32_t z, uint32_t a, uint32_t m) {
    return (a * z) % m;
}

// \gamma(z) = a \cdot (z \mod q) - r \cdot (z \div q)
int32_t y_gamma(int32_t z, uint32_t a, uint32_t m) {
    uint32_t q = LEHMER_QUOTIENT;
    uint32_t r = LEHMER_REMAINDER;
    return (a * (z % q)) - (r * (z / q));
}

// \delta(z) = (z \div q) - (a \cdot z \div m)
int32_t d_delta(int32_t z, uint32_t a, uint32_t m) {
    uint32_t q = LEHMER_QUOTIENT;
    uint32_t r = LEHMER_REMAINDER;
    return (z / q) - ((a * z) / m);
}

// f(z) = \gamma(z) + m \cdot \delta(z)
int32_t f(int32_t z, uint32_t a, uint32_t m) {
    int32_t g = y_gamma(z, a, m);
    int32_t d = d_delta(z, a, m);
    return g + (m * d);
}

/** validate outputs */

// @note params are temporarily void

int test_expected_seeds(void) {}

int test_expected_norms(void) {}

int test_chi_squared(void) {}

const int32_t expected_seeds[MAX_ELEMENTS]
    = {64538327,
       1478293017,
       2095567863,
       86559481,
       1457011992,
       1336393832,
       805362201,
       1889808375,
       2129669881,
       125259599};

const float expected_norms[MAX_ELEMENTS]
    = {0.0300530f,
       0.6883838f,
       0.9758248f,
       0.0403074f,
       0.6784741f,
       0.6223069f,
       0.3750260f,
       0.8800106f,
       0.9917048f,
       0.5832855f};

int main(void) {
    // @note these are inferred from the original equations.
    // no further elaboration or explanation has been discovered.
    const uint32_t m = LEHMER_MODULUS;
    const uint32_t a = LEHMER_MULTIPLIER;

    int32_t z = 1337; // seed

    for (uint32_t i = 0; i < MAX_ELEMENTS; i++) {
        // seed generation?
        z = f(z, a, m);
        // derive bound seed: (int32_t) z % (int32_t) m
        int32_t seed = normalize_int(z, m);
        // normalize seed: (float) z / (float) m
        float norm = normalize_float(z, m);

        // print output
        printf("seed: int = %lu, norm: float = %.7f\n", seed, norm);
    }

    return 0;
}
