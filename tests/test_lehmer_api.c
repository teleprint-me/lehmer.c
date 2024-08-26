/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer_api.c
 *
 * @brief A set of simple tests for validating selecting streams and seeding.
 */

#include "lehmer.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool validate_seed_generation(
    lehmer_state_t* state, uint64_t expected_state, size_t iterations
) {
    lehmer_state_select(state, 0);
    lehmer_seed_set(state, 1);

    for (size_t i = 0; i < iterations; i++) {
        lehmer_generate_modulo(state);
    }

    uint64_t state_value = lehmer_seed_get(state);
    return (state_value == expected_state);
}

bool validate_jump_state(lehmer_state_t* state, uint64_t expected_state) {
    lehmer_state_select(state, 1);
    lehmer_seed_streams(state, 1);

    uint64_t state_value = lehmer_seed_get(state);
    return (state_value == expected_state);
}

// Main test function
int test_lehmer_api(void) {
    bool passed = true;

    lehmer_state_t* state = lehmer_state_create(LEHMER_STREAMS, LEHMER_SEED);

    passed = validate_seed_generation(state, LEHMER_CHECK_SEED, 10000);
    printf(
        "%s: test_lehmer_generate_api: number generation\n",
        passed ? "PASS" : "FAIL"
    );

    passed = passed && validate_jump_state(state, LEHMER_JUMP);
    printf(
        "%s: test_lehmer_generate_api: jump stream\n", passed ? "PASS" : "FAIL"
    );

    lehmer_state_free(state);

    return (passed) ? 0 : 1;
}

int main(void) {
    return test_lehmer_api();
}
