/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer_api.c
 *
 * @brief A set of simple tests for validating selecting streams and seeding.
 */

#include "float_is_close.h"
#include "lehmer.h"
#include "logger.h"

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

/**
 * @brief This test checks if the sequence returns to the initial seed
 * (z_1 = 1) only after (m - 1) iterations, confirming the full period.
 *
 * @brief Test for checking if the multiplier and modulus produce a full period
 *        output for the Lehmer RNG.
 *
 * @note This method can be sped up by applying asynchronous calculations as
 *       this is rather computationally expensive, even in isolation.
 *
 * @warning This test is slow and consumes time and resources. This test should
 * always be executed last to allow fast tests to execute first.
 */
int test_full_period(lehmer_state_t* state) {
    bool passed = true;

    uint32_t count         = 0;
    int32_t  seed          = 1;
    int32_t  original_seed = seed;

    do {
        lehmer_generate_modulo(state);
        seed = lehmer_seed_get(state);
        count++;
    } while (seed != original_seed && count < LEHMER_MODULUS - 1);

    passed = count == (LEHMER_MODULUS - 1);

    if (passed) {
        printf("PASS: Full period detected.\n");
    } else {
        printf("FAIL: %d is less than %d - 1.\n", count, LEHMER_MODULUS);
    }

    return (passed) ? 0 : 1;
}

bool validate_random_seed(lehmer_state_t* state, int32_t expected_seed) {
    for (size_t i = 0; i < 10000; i++) {
        lehmer_generate_modulo(state);
    }
    int32_t current_seed = lehmer_seed_get(state);
    return current_seed == expected_seed;
}

int validate_random_value(lehmer_state_t* state, float expected_output) {
    float current_value = lehmer_seed_normalize(state);
    printf(
        "Expected value: %.9f, Current value: %.9f\n",
        expected_output,
        current_value
    );
    return float_is_close(current_value, expected_output, /*significand*/ 6)
               ? 0
               : 1;
}

int test_seed_generation(lehmer_state_t* state, uint32_t expected_seed) {
    // stream is set to 0
    lehmer_state_select(state, 0);
    // seed is set to 1 in stream 0
    lehmer_seed_set(state, 1);

    // generate 10,000 seeds using a seed of 1 in stream 0
    for (size_t i = 0; i < 10000; i++) {
        lehmer_generate_modulo(state);
    }

    // get the current seed from stream 0
    uint32_t current_seed = lehmer_seed_get(state);
    // assert valid seed
    return (current_seed == expected_seed) ? 0 : 1;
}

int test_jump_state(lehmer_state_t* state, uint32_t expected_seed) {
    // stream is set to 1
    lehmer_state_select(state, 1);
    // reseed remaining streams
    lehmer_seed_streams(state, 1);
    // get the current seed from stream 1
    uint32_t current_seed = lehmer_seed_get(state);
    // assert valid seed
    return (current_seed == expected_seed) ? 0 : 1;
}

// Main test function
int test_lehmer_api(void) {
    bool            passed = true;
    lehmer_state_t* state  = setup_lehmer_state();

    passed = validate_seed_generation(state, LEHMER_CHECK_JUMP);
    if (!passed) {
        LOG_ERROR("FAIL: validate_seed_generation\n");
        lehmer_state_print(state); // print state to stderr for debugging
    }

    passed &= validate_jump_state(state, LEHMER_JUMP);
    if (!passed) {
        LOG_ERROR("FAIL: validate_jump_state\n");
        lehmer_state_print(state);
    }

    passed = validate_random_seed(state, LEHMER_CHECK_SEED);
    if (!passed) {
        LOG_ERROR("FAIL: validate_random_seed\n");
        lehmer_state_print(state);
    }

    passed &= validate_random_value(state, LEHMER_CHECK_VALUE);
    if (!passed) {
        LOG_ERROR("FAIL: validate_random_value\n");
        lehmer_state_print(state);
    }

    lehmer_state_free(state);

    return (passed) ? 0 : 1;
}

int main(void) {
    // both shell and cmake expect 0 on success and 1 on failure
    return test_lehmer_api();
}
