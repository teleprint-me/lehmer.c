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

/**
 * @brief Function to setup the Lehmer RNG state
 *
 * @note The state should always be deterministic which makes the LCG
 * mathematically and computationally sound. This seems counter-intuitive,
 * but is backed up by the maths. This is why it's called a Pseudo Random
 * Number Generator and not a Real Random Number Generator.
 * This is also why it's insecure for use within cryptographic operations.
 * If the parameters are known, then any given sequence is known, making the
 * output predictable.
 */
lehmer_state_t* setup_lehmer_state(void) {
    return lehmer_state_create(LEHMER_STREAMS, LEHMER_SEED);
}

bool validate_seed_generation(lehmer_state_t* state, uint64_t expected_seed) {
    // stream is set to 0
    lehmer_state_select(state, 0);
    // seed is set to 1 in stream 0
    lehmer_seed_set(state, 1);

    // generate 10,000 seeds using a seed of 1 in stream 0
    for (size_t i = 0; i < 10000; i++) {
        lehmer_generate_modulo(state);
    }

    // get the current seed from stream 0
    uint64_t current_seed = lehmer_seed_get(state);
    // assert valid seed
    return (current_seed == expected_seed);
}

bool validate_jump_state(lehmer_state_t* state, uint64_t expected_seed) {
    // stream is set to 1
    lehmer_state_select(state, 1);
    // reseed remaining streams
    lehmer_seed_streams(state, 1);
    // get the current seed from stream 1
    uint64_t current_seed = lehmer_seed_get(state);
    // assert valid seed
    return (current_seed == expected_seed);
}

// Main test function
int test_lehmer_api(void) {
    bool            passed = true;
    lehmer_state_t* state  = setup_lehmer_state();

    passed = validate_seed_generation(state, LEHMER_CHECK_JUMP);
    if (!passed) {
        fprintf(stderr, "FAIL: validate_seed_generation\n");
        lehmer_state_print(state);
    }

    passed &= validate_jump_state(state, LEHMER_JUMP);
    if (!passed) {
        fprintf(stderr, "FAIL: validate_jump_state\n");
        lehmer_state_print(state);
    }

    lehmer_state_free(state);

    return (passed) ? 0 : 1;
}

int main(void) {
    return test_lehmer_api();
}
