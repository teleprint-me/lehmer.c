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
    int32_t* sequence; // The sequence of generated integers
    uint32_t length; // The number of values to generate
    uint32_t position; // The current position in the sequence
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
int32_t lehmer_seed_normalize_to_int(int32_t seed, uint32_t modulus) {
    return (int32_t) ((seed + modulus) % modulus);
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
    state->seed = (0 >= seed) ? LEHMER_SEED : seed;
    state->sequence[0] = generator(seed);

    // Generate the sequence and store it in the stream array
    for (uint32_t i = 1; i < state->length; i++) {
        state->sequence[i] = generator(state->sequence[i - 1]);
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
    state->position = 0;
    // Default to a seed of 123456789 if seed is 0
    state->seed = (0 >= seed) ? LEHMER_SEED : seed;
    // Default to a size of 256 if size is 0
    state->length = (0 >= size) ? LEHMER_SIZE : size;

    // Allocate memory for generating seeds
    state->sequence = (int32_t*) malloc(sizeof(int32_t) * state->length);
    if (NULL == state->sequence) {
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
        if (state->sequence) {
            free(state->sequence);
        }
        free(state);
    }
}

/** State output */

void lehmer_state_print(lehmer_state_t* state) {
    fprintf(stderr, "lehmer->seed: %zu\n", state->seed);
    fprintf(stderr, "lehmer->length: %zu\n", state->length);
    fprintf(stderr, "lehmer->position: %zu\n", state->position);
    fprintf(stderr, "lehmer->sequence:");

    // print the first 10 seeds or size... whichever is less.
    uint32_t boundary = (state->length > 10) ? 10 : state->length;
    for (uint32_t i = 0; i < boundary; i++) {
        fprintf(stderr, " %d,", state->sequence[i]);
    }

    if (state->length > boundary) {
        fprintf(stderr, " ..., %lu - 1\n", state->length);
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
    for (uint32_t i = 0; i < state->length; i++) {
        state->position = i;
        printf("Iteration %zu: Seed = %d\n", i + 1, state->sequence[i]);
        LEHMER_ASSERT_INTEGER(i, expected_stream[i], state->sequence[i]);
    }

    // Normalize the last seed to get the floating-point random number
    int32_t last_seed = state->sequence[state->length - 1];
    float random = lehmer_seed_normalize_to_float(last_seed);
    printf("Random Number = %.7f\n", random);

    // Compare with the expected value (adjust based on corrected expectations)
    float expected = 0.8473225342330162f;
    LEHMER_ASSERT_FLOAT(state->position, expected, random);

    return 0;
}
