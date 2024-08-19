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
void test_lehmer_api(void) {
    bool passed = true;

    uint64_t state_value;
    double   random_value;

    lehmer_state_t* state = lehmer_create_state(STREAMS);

    // Test the default stream (stream 0)
    lehmer_select_stream(state, 0); // Select the default stream
    lehmer_seed_streams(state, 1);  // Set the initial seed for stream 0
    for (long i = 0; i < 10000; i++) {
        random_value = lehmer_generate(state); // Generate random values
    }
    state_value = lehmer_get_seed(state); // Retrieve the final state value
    passed      = (state_value == CHECK); // Validate the state

    // Test stream 1
    lehmer_select_stream(state, 1);                // Select stream 1
    lehmer_seed_streams(state, 1);                 // Set the initial seeds for all streams
    state_value = lehmer_get_seed(state);          // Retrieve the state of stream 1
    passed      = passed && (state_value == A256); // Validate the state

    // Output the result
    if (passed) {
        printf("PASS: test_lehmer_generate_api\n");
    } else {
        printf("FAIL: test_lehmer_generate_api\n");
    }

    lehmer_free_state(state);
}

int main(void) {
    test_lehmer_api();

    return 0;
}
