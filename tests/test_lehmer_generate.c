/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer_generate.c
 *
 * @brief Simple tests for checking the seed and value generation produced by
 *        the Lehmer RNG.
 *
 * @note We don't need to test for randomness. Instead, we're simply validating
 *       the expected outputs.
 */

#include "float_is_close.h"
#include "lehmer.h"

#include <math.h>
#include <stdio.h>
#include <time.h>

/**
 * @brief Function to setup the Lehmer RNG state
 *
 * @note The state should always be deterministic which makes the LCG
 * mathematically and computationally sound. This seems counter-intuitive,
 * but is backed up by the maths. This is why it's called a Pseudo Random
 * Number Generator and not a Pure (or Real) Random Number Generator.
 * This is also why it's insecure for use within cryptographic operations.
 * If the seed is known, then any given sequence is known, making the output
 * predictable.
 */
lehmer_state_t* setup_lehmer_state(void) {
    return lehmer_state_create(LEHMER_STREAMS, LEHMER_SEED);
}

bool validate_random_seed(lehmer_state_t* state, int64_t expected_seed) {
    int64_t seed = lehmer_seed_get(state);
    return seed == expected_seed;
}

// Function to validate the number generation
bool validate_random_value(lehmer_state_t* state, double expected_output) {
    double random_output = lehmer_random_gamma(state);
    return float_is_close(random_output, expected_output, /*significand*/ 6);
}

// Main test function
int test_lehmer_generate(void) {
    bool            passed = true;
    lehmer_state_t* state  = setup_lehmer_state();

    passed = validate_random_value(state, /*expected_output*/ 0.053803);
    if (passed) {
        printf("PASS: test_lehmer_generate_value\n");
    } else {
        printf("FAIL: test_lehmer_generate_value\n");
    }

    lehmer_state_free(state);

    return passed ? 0 : 1; // 0 on success, 1 on failure
}

int main(void) {
    return test_lehmer_generate();
}
