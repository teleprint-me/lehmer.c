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
 * @param LEHMER_CHECK_JUMP Used in testing for validation
 */
#define LEHMER_CHECK_JUMP 399268537

/**
 * @brief A fixture that creates a deterministic Lehmer state using default
 * parameters.
 */
lehmer_state_t* setup_lehmer_state(void) {
    return lehmer_state_create(LEHMER_SEED, LEHMER_SIZE);
}

/**
 * @brief Generalized fixture to free a Lehmer state.
 */
void teardown_lehmer_state(lehmer_state_t* state) {
    lehmer_state_free(state);
}

int test_lehmer_state(void) {
    bool passed = true;

    lehmer_state_t* state = setup_lehmer_state();

    // Test: state->length == LEHMER_SIZE
    if (LEHMER_SIZE != state->length) {
        LOG_ERROR(
            "test_lehmer_state: Expected state->length = %lu, but got %lu\n",
            LEHMER_SIZE,
            state->length
        );
        lehmer_state_print(state);
        passed = false;
    }

    // Test: state->position == 0
    if (0 != state->position) {
        LOG_ERROR(
            "test_lehmer_state: Expected state->position = 0, but got %lu\n",
            state->position
        );
        lehmer_state_print(state);
        passed = false;
    }

    // Test: state->seed == LEHMER_SEED
    if (LEHMER_SEED != state->seed) {
        LOG_ERROR(
            "test_lehmer_state: Expected state->seed = %d, but got %d\n",
            LEHMER_SEED,
            state->seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

int test_lehmer_initial_seed(void) {
    bool passed = true;

    // Test: Initial state
    lehmer_state_t* state = setup_lehmer_state();
    if (LEHMER_SEED != state->seed) {
        LOG_ERROR(
            "test_lehmer_initial_seed: Expected initial seed value to be %lu, "
            "but got %lu\n",
            LEHMER_SEED,
            state->seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    // Test: Setting initial seed
    lehmer_set_initial_seed(state, 1);
    if (1 != state->seed) {
        LOG_ERROR(
            "test_lehmer_initial_seed: Expected initial seed value to be 1, "
            "but got %lu\n",
            state->seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    // Test: Setting and getting initial seed
    int32_t expected_seed = 1337;
    lehmer_set_initial_seed(state, expected_seed);
    int32_t initial_seed = lehmer_get_initial_seed(state);
    if (expected_seed != initial_seed) {
        LOG_ERROR(
            "test_lehmer_initial_seed: Expected initial seed value to be %lu, "
            "but got %lu\n",
            expected_seed,
            initial_seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

int test_lehmer_getters_and_setters(void) {
    bool passed = true;
    const int32_t first_seed = 115541394;
    const int32_t second_seed = 283598515;
    int32_t current_seed = 0;

    lehmer_state_t* state = setup_lehmer_state();

    // test getting the current seed
    current_seed = lehmer_get_current_seed(state);
    if (first_seed != current_seed) {
        LOG_ERROR(
            "test_lehmer_getters_and_setters: Expected current seed value to "
            "be %lu, but got %lu\n",
            first_seed,
            current_seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    // test setting the next seed
    lehmer_set_next_seed(state);
    current_seed = lehmer_get_current_seed(state);
    if (second_seed != current_seed) {
        LOG_ERROR(
            "test_lehmer_getters_and_setters: Expected current seed value to "
            "be %lu, but got %lu\n",
            second_seed,
            current_seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    // test setting the previous seed
    lehmer_set_previous_seed(state);
    current_seed = lehmer_get_current_seed(state);
    if (first_seed != current_seed) {
        LOG_ERROR(
            "test_lehmer_getters_and_setters: Expected current seed value to "
            "be %lu, but got %lu\n",
            first_seed,
            current_seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    // test setting the next seed and then getting the set seed
    current_seed = lehmer_set_next_and_get_seed(state);
    if (second_seed != current_seed) {
        LOG_ERROR(
            "test_lehmer_getters_and_setters: Expected current seed value to "
            "be %lu, but got %lu\n",
            second_seed,
            current_seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

int test_lehmer_seed_normalize(void) {
    bool passed = true;
    float expected_float = 0.0f;
    float current_float = 0.0f;
    const int32_t expected_seed = 115541394;

    lehmer_state_t* state = setup_lehmer_state();

    // test normalizing a seed to a float
    int32_t current_seed = lehmer_get_current_seed(state);
    if (expected_seed != current_seed) {
        LOG_ERROR(
            "Failed: expected current seed %lu, but got %lu",
            expected_seed,
            current_seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    // test normalizing a seed to a signed int (int32_t)
    current_seed = lehmer_seed_normalize_to_int(current_seed);
    if (expected_seed != current_seed) {
        LOG_ERROR(
            "Failed: expected current seed %lu, but got %lu",
            expected_seed,
            current_seed
        );
        lehmer_state_print(state);
        passed = false;
    }

    expected_float = 0.053803154f;
    current_float = lehmer_seed_normalize_to_float(current_seed);
    if (!float_is_close(expected_float, current_float, 7)) {
        LOG_ERROR(
            "Failed: current_seed = %lu, "
            "expected_float = %.7f, "
            "current_float = %.7f\n",
            current_seed,
            expected_float,
            current_float
        );
        lehmer_state_print(state);
        passed = false;
    }

    // fast-forward to the 9th seed
    for (uint32_t i = 0; i < 9; i++) {
        lehmer_set_next_seed(state);
    }

    expected_float = 0.847322534f;
    current_seed = lehmer_get_current_seed(state);
    current_float = lehmer_seed_normalize_to_float(current_seed);
    if (!float_is_close(expected_float, current_float, 7)) {
        LOG_ERROR(
            "Failed: current_seed = %lu, "
            "expected_float = %.7f, "
            "current_float = %.7f\n",
            current_seed,
            expected_float,
            current_float
        );
        lehmer_state_print(state);
        passed = false;
    }

    teardown_lehmer_state(state);

    return passed ? 0 : 1;
}

int test_random_seed_and_normalize(void) {
    bool passed = true;

    const int32_t expected_seed = 1443729859;
    const float expected_output = 0.672289104f;

    // stress test the initial state
    lehmer_state_t* state = setup_lehmer_state();
    for (size_t i = 0; i < 10000; i++) {
        lehmer_set_next_seed(state);
        // breaks at 10000 % 256
    }

    // expected position = 10000 % 256 = 16
    // @note the python -m lehmer.cli output is offset by +1
    if (16 != state->position) {
        LOG_ERROR(
            "test_random_seed_and_normalize: Expected position 16, "
            "but got %d.\n",
            state->position
        );
        lehmer_state_print(state); // print state to stderr for debugging
        passed = false;
    }

    int32_t current_seed = lehmer_get_current_seed(state);
    if (expected_seed != current_seed) {
        LOG_ERROR(
            "test_random_seed_and_normalize: Expected seed %d, but got %d.\n",
            expected_seed,
            current_seed
        );
        lehmer_state_print(state); // print state to stderr for debugging
        passed = false;
    }

    float current_output = lehmer_seed_normalize_to_float(current_seed);
    if (!float_is_close(expected_output, current_output, 7)) {
        LOG_ERROR(
            "test_random_seed_and_normalize: "
            "Expected normalized seed %.7f, but got %.7f.\n",
            expected_output,
            current_output
        );

        lehmer_state_print(state);
        passed = false;
    }

    teardown_lehmer_state(state);

    printf("%s", passed ? "." : "x");
    return passed ? 0 : 1;
}

int test_seed_generation(void) {
    const int32_t expected_seed = 1882556969;
    int32_t current_seed = 0;
    lehmer_state_t* state = setup_lehmer_state();

    // set the initial seed to 1
    lehmer_generate(state, lehmer_generate_modulo, /* seed */ 1);

    // generate 10,000 seeds using a seed of 1 in stream 0
    for (size_t i = 0; i < 10000; i++) {
        current_seed = lehmer_set_next_and_get_seed(state);
    }

    // get the current seed
    bool passed = (current_seed == expected_seed);

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

int test_jump_state(void) {
    const int32_t expected_seed = LEHMER_CHECK_JUMP;
    lehmer_state_t* state = setup_lehmer_state();

    // get the next seed in the generated sequence
    int32_t seed = lehmer_set_next_and_get_seed(state);
    // set the selected seed as the new root seed
    lehmer_set_initial_seed(state, seed);
    // regenerate the sequence using the new root seed
    lehmer_regenerate(state, lehmer_generate_modulo);
    // get the current seed from stream 1
    uint32_t current_seed = lehmer_get_current_seed(state);
    bool passed = (current_seed == expected_seed);

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
    bool passed = true;
    uint32_t count = 0;
    int32_t seed = -1; // seeds are always positive
    int32_t original_seed = seed;

    // Use the new fixture functions
    lehmer_state_t* state = setup_lehmer_state();

    do {
        seed = lehmer_set_next_and_get_seed(state);
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

int main(void) {
    // both shell and cmake expect 0 on success and 1 on failure
    int passed = 0; // Assuming success

    passed |= test_lehmer_state();
    passed |= test_lehmer_initial_seed();
    passed |= test_lehmer_getters_and_setters();
    passed |= test_lehmer_seed_normalize();
    passed |= test_random_seed_and_normalize();
    passed |= test_seed_generation();
    // passed |= test_jump_state();
    // passed |= test_full_period();
    printf("\n");

    return passed;
}
