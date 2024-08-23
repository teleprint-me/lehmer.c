/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer_generate.c
 *
 * @brief A simple suite of tests for the Lehmer RNG in pure C
 */

#include "lehmer.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Function to setup the Lehmer RNG state
lehmer_state_t* setup_lehmer_state(void) {
    lehmer_state_t* state = lehmer_create_state(STREAMS);
    lehmer_seed_streams(state, DEFAULT);
    return state;
}

// Function to validate the number generation
bool validate_number_generation(
    lehmer_state_t* state, uint64_t expected_state, size_t iterations
) {
    double random_value;

    lehmer_select_stream(state, 0);
    lehmer_set_seed(state, 1);

    for (size_t i = 0; i < iterations; i++) {
        random_value = lehmer_generate(state);
    }

    uint64_t state_value = lehmer_get_seed(state);
    return (state_value == expected_state);
}

// Function to validate the jump state
bool validate_jump_state(lehmer_state_t* state, uint64_t expected_state) {
    lehmer_select_stream(state, 1);
    lehmer_seed_streams(state, 1);

    uint64_t state_value = lehmer_get_seed(state);
    return (state_value == expected_state);
}

// Main test function
int test_lehmer_api(void) {
    bool passed = true;

    lehmer_state_t* state = setup_lehmer_state();

    passed = validate_number_generation(state, CHECK, 10000);
    printf(
        "%s: test_lehmer_generate_api: number generation\n",
        passed ? "PASS" : "FAIL"
    );

    passed = passed && validate_jump_state(state, A256);
    printf(
        "%s: test_lehmer_generate_api: jump stream\n", passed ? "PASS" : "FAIL"
    );

    lehmer_free_state(state);

    return (passed) ? 0 : 1;
}

int main(void) {
    return test_lehmer_api();
}
