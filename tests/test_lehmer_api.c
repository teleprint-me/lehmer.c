/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer_generate.c
 *
 * @brief A simple suite of tests for the Lehmer RNG in pure C
 */

#include "lehmer.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// this should pass, but fails
int test_lehmer_api(void) {
    bool passed = true;

    double random_value;

    /* setup */

    // Initialize the state using the predefined number of streams
    lehmer_state_t* state = lehmer_create_state(STREAMS); // STREAMS = 256
    // Set the initial state to seed the streams using the default value
    lehmer_seed_streams(state, DEFAULT); // DEFAULT = 123456789

    /* initialization */

    // Select the default stream
    lehmer_select_stream(state, 0); // first stream
    // Set the initial seed for stream 0
    lehmer_set_seed(state, 1); // value is 1

    /* generation */

    for (size_t i = 0; i < 10000; i++) {
        // Generate random values
        random_value = lehmer_generate(state);
    }

    /* validation */

    /* test 1: validate number generation */

    // Retrieve the final state value
    uint64_t state_value = lehmer_get_seed(state);
    // Validate the state
    passed               = (state_value == CHECK);

    // Output test 1 results
    if (passed) {
        printf("PASS: test_lehmer_generate_api: number generation\n");
    } else {
        printf("FAIL: test_lehmer_generate_api: number generation\n");
    }

    /* test 2: validate jump states */

    // Test stream 1
    lehmer_select_stream(state, 1); // second stream
    // reset the streams by reseeding them with a value of 1
    lehmer_seed_streams(state, 1);
    // Retrieve the state of stream 1
    state_value = lehmer_get_seed(state);
    // Validate the state; note, previous test affects current test
    // tests should be isolated/encapsulated
    passed      = passed && (state_value == A256);

    // Output the result
    if (passed) {
        printf("PASS: test_lehmer_generate_api: jump stream\n");
    } else {
        printf("FAIL: test_lehmer_generate_api: jump stream\n");
    }

    lehmer_free_state(state);

    return (passed) ? 0 : 1;
}

int main(void) {
    return test_lehmer_api();
}
