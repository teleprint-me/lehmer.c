/**
 * Copyright © 2024 Austin Berrio
 *
 * @file tests/test_lehmer.c
 *
 * @brief A suite of simple tests for validating the Lehmer LCG PRNG API.
 *
 * @note Keep fixtures and tests as simple as reasonably possible.
 *       The simpler, the better.
 */

#include "float_is_close.h"
#include "lehmer.h"
#include "logger.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @brief A fixture that creates a deterministic Lehmer state using default
 * parameters.
 */
lehmer_state_t* setup_lehmer_state(void) {
    lehmer_state_t* state = lehmer_state_create(LEHMER_STREAMS, LEHMER_SEED);
    return state;
}

/**
 * @brief Generalized fixture to free a Lehmer state.
 */
void teardown_lehmer_state(lehmer_state_t* state) {
    if (state) {
        lehmer_state_free(state);
    }
}

/**
 * @brief Tests the construction and destruction of a Lehmer state
 */
int test_lehmer_state(void) {
    bool passed = true;

    lehmer_state_t* state = setup_lehmer_state();

    // Test: state->size == LEHMER_STREAMS
    if (state->size != LEHMER_STREAMS) {
        LOG_ERROR(
            "\nFailed: Expected size to be %u, but got %u",
            LEHMER_STREAMS,
            state->size
        );
        lehmer_state_print(state);
        passed = false;
    }

    // Test: state->stream == 0
    if (state->stream != 0) {
        LOG_ERROR(
            "\nFailed: Expected stream to be 0, but got %u", state->stream
        );
        lehmer_state_print(state);
        passed = false;
    }

    // Test: state->seed[0] == LEHMER_SEED
    if (state->seed[0] != LEHMER_SEED) {
        LOG_ERROR(
            "\nFailed: Expected initial seed to be %d, but got %d",
            LEHMER_SEED,
            state->seed[0]
        );
        lehmer_state_print(state);
        passed = false;
    }

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

test_lehmer_state_select(void) {
    bool passed = true;

    lehmer_state_t* state = setup_lehmer_state();

    // @todo select a stream for the state
    // default is 0, so we slecect the next stream
    // lehmer_state_select(state, 1)
    // @todo validate the stream is selected
    // state->stream == 1
    // @todo test for overflow and ensure values wrap around
    // @note I don't know what the values are for this stream. will need to
    // determine what the valid values are regardless.

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

/**
 * @brief Tests if the Lehmer RNG generates numbers in its full period
 *
 * This function checks whether the sequence returns to its initial seed
 * (z_1 = 1) after exactly m - 1 iterations, confirming that it has reached its
 * full period. The test is crucial as it ensures the Lehmer RNG's output is
 * not predictable and remains unbiased over time.
 *
 * @return 0 on success and 1 on failure.
 *
 * @warning This test unit is slow and consumes time and resources. This test
 * should always be executed last to allow fast tests to execute first.
 */
int test_full_period(void) {
    bool     passed        = true;
    uint32_t count         = 0;
    int32_t  seed          = 1;
    int32_t  original_seed = seed;

    // Use the new fixture functions
    lehmer_state_t* state = setup_lehmer_state();

    do {
        lehmer_generate_modulo(state);
        seed = lehmer_seed_get(state);
        count++;
    } while (seed != original_seed && count < LEHMER_MODULUS - 1);

    passed = count == (LEHMER_MODULUS - 1);

    if (!passed) {
        LOG_ERROR(
            "test_full_period: %d is less than %d - 1.\n",
            count,
            LEHMER_MODULUS
        );
        lehmer_state_print(state); // print state to stderr for debugging
    }

    // Teardown
    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

/**
 * @brief Verifies that the Lehmer RNG generates seeds as expected
 *
 * This function checks if generating 10,000 random numbers using a given state
 * results in the final seed matching an expected value. Ensuring correct seed
 * generation is crucial for maintaining deterministic output and
 * reproducibility across different runs of the Lehmer RNG.
 *
 * @return 0 on success and 1 on failure.
 */
int test_random_seed(void) {
    const int32_t   expected_seed = LEHMER_CHECK_SEED;
    lehmer_state_t* state         = setup_lehmer_state();

    for (size_t i = 0; i < 10000; i++) {
        lehmer_generate_modulo(state);
    }

    int32_t current_seed = lehmer_seed_get(state);
    bool    passed       = current_seed == expected_seed;

    if (!passed) {
        LOG_ERROR(
            "test_random_seed: Expected seed %d, but got %d.\n",
            expected_seed,
            current_seed
        );
        lehmer_state_print(state); // print state to stderr for debugging
    }

    teardown_lehmer_state(state);

    return passed ? 0 : 1;
}

/**
 * @brief Verifies the Lehmer RNG generates numbers close to the given
 * expected output
 *
 * This function checks if a generated random value is within a specified
 * tolerance of an expected output. Ensuring correctness in generating random
 * values is crucial for maintaining unbiased and uniformly distributed
 * sequences that can be used across various applications, including
 * simulations and statistical analysis.
 *
 * @return 0 on success and 1 on failure.
 */
int test_random_value(void) {
    float           expected_output = LEHMER_CHECK_VALUE;
    lehmer_state_t* state           = setup_lehmer_state();
    float           current_value   = lehmer_seed_normalize(state);

    bool passed
        = float_is_close(current_value, expected_output, /* significand */ 6);

    if (!passed) {
        LOG_ERROR(
            "test_random_value: Expected output: %.7f, actual output: %.7f.\n",
            expected_output,
            current_value
        );

        lehmer_state_print(state);
    }

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

/**
 * @brief Verifies that Lehmer RNG correctly generates seeds based on input
 * streams and seeds
 *
 * This function tests if generating 10,000 random numbers using a given state
 * with a specific stream (stream 0) and seed (1) results in the final seed
 * matching an expected value. Ensuring correctness in seed generation is
 * crucial for maintaining deterministic output and reproducibility across
 * different runs of the Lehmer RNG, especially when switching between streams
 * or restarting the sequence from a specific point.
 *
 * @return 0 on success and 1 on failure.
 */
int test_seed_generation(void) {
    const int32_t   expected_seed = LEHMER_CHECK_SEED;
    lehmer_state_t* state         = setup_lehmer_state();

    // stream is set to 0
    lehmer_state_select(state, 0);
    // seed is set to 1 in stream 0
    lehmer_seed_set(state, 1);

    // generate 10,000 seeds using a seed of 1 in stream 0
    for (size_t i = 0; i < 10000; i++) {
        lehmer_generate_modulo(state);
    }

    // get the current seed from stream 0
    int32_t current_seed = lehmer_seed_get(state);
    bool    passed       = (current_seed == expected_seed);

    if (!passed) {
        LOG_ERROR(
            "test_seed_generation: Expected seed %d, but got %d.\n",
            expected_seed,
            current_seed
        );
        lehmer_state_print(state); // print state to stderr for debugging
    }

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

/**
 * @brief Verifies that Lehmer RNG can jump to a specific stream and seed
 *
 * This function tests if the state can be correctly set to a specified
 * stream (stream 1) and reseeded with an expected value. Ensuring correctness
 * in jumping between streams is crucial for maintaining deterministic output
 * when switching between different sequences, as well as ensuring that all
 * remaining streams are reset properly before the test.
 *
 * @return 0 on success and 1 on failure.
 */
int test_jump_state(void) {
    const int32_t   expected_seed = LEHMER_CHECK_JUMP;
    lehmer_state_t* state         = setup_lehmer_state();

    // stream is set to 1
    lehmer_state_select(state, 1);
    // reseed remaining streams
    lehmer_seed_streams(state, 1);
    // get the current seed from stream 1
    uint32_t current_seed = lehmer_seed_get(state);
    bool     passed       = (current_seed == expected_seed);

    if (!passed) {
        LOG_ERROR(
            "test_jump_state: Expected seed %d, but got %d.\n",
            expected_seed,
            current_seed
        );
        lehmer_state_print(state); // print state to stderr for debugging
    }

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

int main(void) {
    // both shell and cmake expect 0 on success and 1 on failure
    int passed = 0; // Assuming success

    passed |= test_lehmer_state();
    // passed |= test_random_seed();
    // passed |= test_random_value();
    // passed |= test_seed_generation();
    // passed |= test_jump_state();
    // passed |= test_full_period();
    printf("\n");

    return passed;
}
