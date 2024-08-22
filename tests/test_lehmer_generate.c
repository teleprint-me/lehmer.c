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

// Function to setup the Lehmer RNG state
lehmer_state_t* setup_lehmer_state(uint64_t seed) {
    lehmer_state_t* state = lehmer_create_state(STREAMS); // STREAMS = 256
    lehmer_seed_streams(state, seed); // DEFAULT = 123456789
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
    size_t significand     = 6;
    double expected_output = 0.053803; // this is right
    // double expected_output = 0.055780; // this is wrong

    lehmer_state_t* state = setup_lehmer_state(DEFAULT);
    bool            passed
        = validate_lehmer_generator(state, expected_output, significand);

    // Output the result
    if (passed) {
        printf("PASS: test_lehmer_generate_value\n");
    } else {
        printf("FAIL: test_lehmer_generate_value\n");
        // Output results due to potential hidden discrepancies
        printf("test_lehmer_generate_value: Expected: %f\n", expected_output);
        // printf("test_lehmer_generate_value: Actual: %f\n", random);
    }

    lehmer_free_state(state);

    return passed ? 0 : 1; // 0 on success, 1 on failure
}

int main(void) {
    return test_lehmer_generate();
}
