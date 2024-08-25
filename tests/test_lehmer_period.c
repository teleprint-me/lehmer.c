/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer_period.c
 *
 * @brief Test for checking if the multiplier and modulus produce a full period
 *        output for the Lehmer RNG.
 *
 * @note This method can be sped up by applying asynchronous calculations as
 *       this is rather computationally expensive, even in isolation.
 */

#include "lehmer.h"

#include <stdbool.h>
#include <stdio.h>

long lehmer_rng(long seed) {
    return (seed * LEHMER_MULTIPLIER) % LEHMER_MODULUS;
}

/**
 * @brief This test checks if the sequence returns to the initial seed
 * (z_1 = 1) only after (m - 1) iterations, confirming the full period.
 */
int test_full_period(void) {
    bool passed = true;

    long count         = 0;
    long seed          = 1;
    long original_seed = seed;

    do {
        seed = lehmer_rng(seed);
        count++;
    } while (seed != original_seed && count < LEHMER_MODULUS - 1);

    passed = count == (LEHMER_MODULUS - 1);

    if (passed) {
        printf("PASS: Full period detected.\n");
    } else {
        printf("FAIL: %d is less than %d - 1.\n", count, LEHMER_MODULUS);
    }

    return (passed) ? 0 : 1;
}

int main() {
    return test_full_period();
}
