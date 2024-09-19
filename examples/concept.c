/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/concept.c
 *
 * @brief Minimal conceptual example showcasing the core API.
 *
 * @note gcc -o concept examples/concept.c -lm -g
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
    int32_t seed; // The initial seed used to start the sequence
    int32_t* stream; // The sequence of generated integers
    uint32_t size; // The number of values to generate
    uint32_t index; // The current index in the sequence
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
    int64_t scale = (int64_t) a * z; // Use int64_t to avoid overflow
    int32_t r = (int32_t) (scale % m); // Modulus operation
    return lehmer_seed_normalize_to_int(r, m);
}

// Generate the sequence and store it in the stream array
void lehmer_generate(
    lehmer_state_t* state, lehmer_generate_t generator, int32_t seed
) {
    // Set the initial seed
    state->seed = seed;
    state->stream[0] = generator(seed);

    // Generate the sequence and store it in the stream array
    for (uint32_t i = 1; i < state->size; i++) {
        state->stream[i] = generator(state->stream[i - 1]);
    }
}

// Create and initialize the state with dynamic seed generation
lehmer_state_t* lehmer_state_create(int32_t seed, uint32_t size) {
    // Allocate memory for managing the LCG PRNG state
    lehmer_state_t* state = (lehmer_state_t*) malloc(sizeof(lehmer_state_t));
    if (NULL == state) {
        return NULL;
    }

    // Zero-initialize the index
    state->index = 0;
    // Default to a seed of 123456789 if seed is 0
    state->seed = (0 >= seed) ? LEHMER_SEED : seed;
    // Default to a size of 256 if size is 0
    state->size = (0 >= size) ? LEHMER_SIZE : size;

    // Allocate memory for generating seeds
    state->stream = (int32_t*) malloc(sizeof(int32_t) * state->size);
    if (NULL == state->stream) {
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
        if (state->stream) {
            free(state->stream);
        }
        free(state);
    }
}

/** State output */

void lehmer_state_print(lehmer_state_t* state) {
    fprintf(stderr, "lehmer->size: %zu\n", state->size);
    fprintf(stderr, "lehmer->index: %zu\n", state->index);
    fprintf(stderr, "lehmer->seed: %zu\n", state->seed);
    fprintf(stderr, "lehmer->stream:");

    // print the first 10 seeds or size... whichever is less.
    uint32_t boundary = (state->size > 10) ? 10 : state->size;
    for (uint32_t i = 0; i < boundary; i++) {
        fprintf(stderr, " %d,", state->stream[i]);
    }

    if (state->size > boundary) {
        fprintf(stderr, " ..., %lu - 1\n", state->size);
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
            "Iteration %zu: Expected %d, Got %d\n", \
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
            "Iteration %zu: Expected %.7f, Got %.7f\n", \
            iteration, \
            expected, \
            current \
        ); \
        exit(1); \
    }

int main(void) {
    // Initialize the Lehmer RNG state
    lehmer_state_t* state = lehmer_state_create(LEHMER_SEED, LEHMER_SIZE);
    lehmer_state_print(state);

    // Correct the index handling, ensuring the expected seeds match
    for (uint32_t i = 0; i < state->size; i++) {
        state->index = i;
        printf("Iteration %zu: Seed = %d\n", i + 1, state->stream[i]);
        LEHMER_ASSERT_INTEGER(i, expected_stream[i], state->stream[i]);
    }

    // Normalize the last seed to get the floating-point random number
    int32_t last_seed = state->stream[state->size - 1];
    float random = lehmer_seed_normalize_to_float(last_seed);
    printf("Random Number = %.7f\n", random);

    // Compare with the expected value (adjust based on corrected expectations)
    float expected = 0.8473225342330162f;
    LEHMER_ASSERT_FLOAT(state->index, expected, random);

    return 0;
}
