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

int test_lehmer_generate(void) {
    // double expected = 0.055780; // this is wrong
    double expected = 0.053803; // this is right

    // Initialize the state using the predefined number of streams
    lehmer_state_t* state = lehmer_create_state(STREAMS); // STREAMS = 256
    // Set the initial state to seed the streams using the default value
    lehmer_seed_streams(state, DEFAULT); // DEFAULT = 123456789

    double random = lehmer_generate(state);
    bool   passed = float_is_close(random, expected, /* significand */ 6);

    // Output the result
    if (passed) {
        printf("PASS: test_lehmer_generate_value\n");
    } else {
        printf("FAIL: test_lehmer_generate_value\n");
        // Output results due to potential hidden discrepancies
        printf("test_lehmer_generate_value: Expected: %f\n", expected);
        printf("test_lehmer_generate_value: Actual: %f\n", random);
    }

    lehmer_free_state(state);

    return passed ? 0 : 1; // 0 on success, 1 on failure
}

int main(void) {
    return test_lehmer_generate();
}
