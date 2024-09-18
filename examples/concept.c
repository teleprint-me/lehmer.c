/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/concept.c
 *
 * @brief Minimal conceptual example showcasing the core API.
 *
 * gcc -o concept examples/concept.c
 */

#include <math.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/** Simplified conceptual API */

#define LEHMER_MODULUS 2147483647 // Mersenne prime number (2^31 - 1)
#define LEHMER_MULTIPLIER 48271 // Prime number
#define LEHMER_SEED 123456789 // Default seed
#define LEHMER_SIZE 10 // Number of values to generate

typedef struct LehmerState {
    int32_t* seed; // Array of seeds representing the initial state
    uint32_t size; // The number of seeds (upper limit)
    uint32_t index; // The selected seed to generate sequences from
} lehmer_state_t;

/**
 * @brief Callback function for generating new seeds in the Lehmer RNG.
 *
 * @param state The current state object.
 */
typedef int32_t (*lehmer_generate_t)(int32_t);

// @brief Normalizes a seed to a float in the range 0.0 to 1.0
float lehmer_seed_normalize_to_float(int32_t seed) {
    return (float) seed / (float) LEHMER_MODULUS;
}

// @brief Normalizes a seed to a integer in the range 0 to m - 1
int32_t lehmer_seed_normalize_to_int(int32_t value, uint32_t modulus) {
    return (int32_t) ((value + modulus) % modulus);
}

// Generate the next random number in the sequence
int32_t lehmer_generate_modulo(int32_t z) {
    const uint32_t a = LEHMER_MULTIPLIER;
    const uint32_t m = LEHMER_MODULUS;

    // Lehmer formula: (a * z) % m
    int64_t temp = (int64_t) a * z; // Use int64_t to avoid overflow
    int32_t r = (int32_t) (temp % m); // Modulus operation
    return lehmer_seed_normalize_to_int(r, m);
}

// Generalized Lehmer sequence generator
void lehmer_generate(
    lehmer_state_t* state, lehmer_generate_t generator, int32_t seed
) {
    // Initialize the first seed
    state->seed[0] = seed;

    // Generate the sequence of seeds
    for (uint32_t i = 1; i < state->size; i++) {
        // Use the previous seed to generate the next
        int32_t previous = state->seed[i - 1];
        state->seed[i] = generator(previous);
    }
}

// Create and initialize the state with dynamic seed generation
lehmer_state_t* lehmer_state_create(uint32_t size, int32_t seed) {
    // Allocate memory for managing the LCG PRNG state
    lehmer_state_t* state = (lehmer_state_t*) malloc(sizeof(lehmer_state_t));
    if (NULL == state) {
        return NULL;
    }

    // Initialize the index to 0
    state->index = 0;
    // Default to a size of 256 if size is 0
    state->size = (0 >= size) ? LEHMER_SIZE : size;

    // Allocate memory for generating seeds
    state->seed = (int32_t*) malloc(sizeof(int32_t) * state->size);
    if (NULL == state->seed) {
        free(state);
        return NULL;
    }

    // Initialize the state object
    lehmer_generate(state, lehmer_generate_modulo, seed);

    return state;
}

// Free the allocated memory
void lehmer_state_free(lehmer_state_t* state) {
    if (state) {
        if (state->seed) {
            free(state->seed);
        }
        free(state);
    }
}

/** State output */

void lehmer_state_print(lehmer_state_t* state) {
    fprintf(stderr, "lehmer->size: %zu\n", state->size);
    fprintf(stderr, "lehmer->index: %zu\n", state->index);
    fprintf(stderr, "lehmer->seed:");
    // print the first 10 seeds or size... whichever is less.
    uint32_t boundary = (state->size > 10) ? 10 : state->size;
    for (uint32_t i = 0; i < boundary; i++) {
        fprintf(stderr, " %lu,", state->seed[i]);
    }
    if (state->size > boundary) {
        fprintf(stderr, " ..., %u - 1\n", state->size);
    } else {
        fprintf(stderr, " ...\n");
    }
}

/** Debugging */

// @brief Valid stream for selected state at stream 0 using LEHMER_SEED
// @ref python -m lehmer.cli -v -i 10 -r 0
// `python -m lehmer.cli -h` for more info
int32_t expected_stream[LEHMER_SIZE] = {
    115541394,
    283598515,
    1523151587,
    652633738,
    1845549155,
    291648857,
    1426670162,
    1289797906,
    2136310349,
    1819611286,
};

#define LEHMER_ASSERT_INTEGER(iteration, expected, current) \
    if ((expected) != (current)) { \
        fprintf( \
            stderr, \
            "Iteration %u: Expected %d, Got %d\n", \
            iteration, \
            expected, \
            current \
        ); \
        exit(1); \
    }

#define LEHMER_ASSERT_FLOAT(iteration, expected, current) \
    if (!((fabsf(expected - current)) < (0.000001f))) { \
        fprintf( \
            stderr, \
            "Iteration %u: Expected %f, Got %f\n", \
            iteration, \
            expected, \
            current \
        ); \
        exit(1); \
    }

int main(void) {
    lehmer_state_t* state = lehmer_state_create(LEHMER_SIZE, LEHMER_SEED);

    lehmer_state_print(state);

    // Generate and print Lehmer RNG sequence
    for (uint32_t i = 1; i < state->size; i++) {
        state->index = i;
        printf("Iteration %u: Seed = %d\n", i, state->seed[i]);
        LEHMER_ASSERT_INTEGER(i, expected_stream[i], state->seed[i]);
    }

    // last seed
    int32_t seed = state->seed[state->index];
    float random = lehmer_seed_normalize_to_float(seed);
    float expected = 0.847322534f;
    printf("Random Number = %.7f\n", random);
    LEHMER_ASSERT_FLOAT(state->index, expected, random);

    lehmer_state_free(state);

    return 0;
}
