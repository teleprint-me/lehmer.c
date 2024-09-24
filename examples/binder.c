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

int32_t normalize_int(int32_t z, uint32_t m) {
    return (int32_t) z % (int32_t) m;
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
    int32_t q = (int32_t) (m / a);
    int32_t r = (int32_t) (m % a);
    return (a * (z % q)) - (r * (z / q));
}

// \delta(z) = (z \div q) - (a \cdot z \div m)
int32_t d_delta(int32_t z, uint32_t a, uint32_t m) {
    int32_t q = (int32_t) (m / a);
    int32_t r = (int32_t) (m % a);
    return ((z / q) - (a * z / m));
}

// f(z) = \gamma(z) + m \cdot \delta(z)
int32_t f(int32_t z, uint32_t a, uint32_t m) {
    int32_t g = y_gamma(z, a, m);
    int32_t d = d_delta(z, a, m);
    return g + (m * d);
}

int main(void) {
    // @note these are inferred from the original equations.
    // no further elaboration or explanation has been discovered.
    const uint32_t m = 2147483647; // modulus
    const uint32_t a = 48271; // multiplier
    const int32_t q = (int32_t) (m / a); // quotient
    const int32_t r = (int32_t) (m % a); // remainder

    int32_t z = 1337; // seed

    for (uint32_t i = 0; i < 10; i++) {
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
