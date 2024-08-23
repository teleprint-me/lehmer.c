/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer_period.c
 *
 * @brief Concept for testing a full period of the Lehmer RNG
 */

#include <stdbool.h>
#include <stdio.h>

#define MODULUS    2147483647
#define MULTIPLIER 16807

long lehmer_rng(long seed) {
    return (seed * MULTIPLIER) % MODULUS;
}

/**
 * @brief This test checks if the sequence returns to the initial seed
 * (z_1 = 1) only after (m - 1) iterations, confirming the full period.
 */
void test_full_period() {
    bool passed = true;

    long seed          = 1;
    long original_seed = seed;
    int  count         = 0;

    do {
        seed = lehmer_rng(seed);
        count++;
    } while (seed != original_seed && count < MODULUS - 1);

    if (count == MODULUS - 1) {
        printf("Test passed: Full period detected.\n");
    } else {
        printf(
            "Test failed: Period detected was %d, which is less than MODULUS "
            "- 1.\n",
            count
        );
    }
}

int main() {
    test_full_period();
    return 0;
}
