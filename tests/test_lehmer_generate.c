/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer_generate.c
 *
 * @brief A simple suite of tests for the Lehmer RNG in pure C
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
    lehmer_state_t* state = lehmer_create_state(STREAMS); // STREAMS = 256
    lehmer_seed_streams(state, DEFAULT); // DEFAULT = 123456789
    return state;
}

// Function to validate the number generation
bool validate_lehmer_generator(
    lehmer_state_t* state, double expected_output, size_t significand
) {
    double random_output = lehmer_generate(state);
    return float_is_close(random_output, expected_output, significand);
}

// Main test function
int test_lehmer_generate(void) {
    bool passed = true;

    // 0.053803 is a deterministic value that is always produced
    // when STREAMS = 256 and DEFAULT = 123456789
    double expected_output = 0.053803;
    size_t significand     = 6;

    lehmer_state_t* state = setup_lehmer_state();

    passed = validate_lehmer_generator(state, expected_output, significand);

    // Output the result
    if (passed) {
        printf("PASS: test_lehmer_generate_value\n");
    } else {
        printf("FAIL: test_lehmer_generate_value\n");
    }

    lehmer_free_state(state);

    return passed ? 0 : 1; // 0 on success, 1 on failure
}

int main(void) {
    return test_lehmer_generate();
}
