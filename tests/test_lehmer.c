/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer.c
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
 * @brief Creates a Lehmer RNG state with default parameters
 */
lehmer_state_t* create_deterministic_lehmer_state(void) {
    return lehmer_state_create(LEHMER_STREAMS, LEHMER_SEED);
}

/**
 * @brief Test if the Lehmer RNG produces a full period cycle, returning to the
 * initial seed after a specific number of iterations.
 *
 * This test checks if the sequence returns to the initial seed (z_1 = 1) only
 * after (m - 1) iterations, confirming the full period.
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

    if (!passed) {
        LOG_ERROR("FAIL: %d is less than %d - 1.\n", count, LEHMER_MODULUS);
    }

    printf("%s", passed ? "." : "x");
    return (passed) ? 0 : 1;
}

/**
 * @brief Generate 10,000 random numbers and checks if the final seed matches
 * the expected one.
 *
 * @return 0 on success and 1 on failure.
 */
int test_random_seed(lehmer_state_t* state, int32_t expected_seed) {
    for (size_t i = 0; i < 10000; i++) {
        lehmer_generate_modulo(state);
    }
    int32_t current_seed = lehmer_seed_get(state);
    bool    passed       = current_seed == expected_seed;

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

/**
 * @brief Check if a generated random value is close to the given expected
 * output.
 */
int test_random_value(lehmer_state_t* state, float expected_output) {
    float current_value = lehmer_seed_normalize(state);
    printf(
        "Expected value: %.9f, Current value: %.9f\n",
        expected_output,
        current_value
    );
    bool passed
        = float_is_close(current_value, expected_output, /*significand*/ 6);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

/**
 * @brief Test the seed generation process for a specific stream and seed.
 */
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
    bool     passed       = (current_seed == expected_seed);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

/**
 * @brief Test the jump to a specific stream and seed.
 */
int test_jump_state(lehmer_state_t* state, uint32_t expected_seed) {
    // stream is set to 1
    lehmer_state_select(state, 1);
    // reseed remaining streams
    lehmer_seed_streams(state, 1);
    // get the current seed from stream 1
    uint32_t current_seed = lehmer_seed_get(state);
    bool     passed       = (current_seed == expected_seed);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

int main(void) {
    // both shell and cmake expect 0 on success and 1 on failure
    int             passed = 0; // Assuming success
    lehmer_state_t* state  = create_deterministic_lehmer_state();

    passed |= validate_seed_generation(state, LEHMER_CHECK_JUMP);
    if (!passed) {
        LOG_ERROR("FAIL: validate_seed_generation\n");
        lehmer_state_print(state); // print state to stderr for debugging
    }

    passed |= validate_jump_state(state, LEHMER_JUMP);
    if (!passed) {
        LOG_ERROR("FAIL: validate_jump_state\n");
        lehmer_state_print(state);
    }

    passed |= validate_random_seed(state, LEHMER_CHECK_SEED);
    if (!passed) {
        LOG_ERROR("FAIL: validate_random_seed\n");
        lehmer_state_print(state);
    }

    passed |= validate_random_value(state, LEHMER_CHECK_VALUE);
    if (!passed) {
        LOG_ERROR("FAIL: validate_random_value\n");
        lehmer_state_print(state);
    }

    lehmer_state_free(state);

    return (passed) ? 0 : 1;
}
