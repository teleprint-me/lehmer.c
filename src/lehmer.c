/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/lehmer.c
 *
 * @brief A 32-bit implementation of the Lehmer RNG in pure C
 *
 * @ref Random number generators: good ones are hard to find
 * @cite https://dl.acm.org/doi/10.1145/63039.63042
 * @cite https://www.cs.wm.edu/~va/software/park/park.html
 *
 * @ref Discrete Mathematics
 * @cite 1.2 Binomial Coefficients for definitions
 * @cite 5.1 Generating Functions for details
 *
 * @ref Introduction to Probability and Mathematical Statistics
 * @cite 9.3: The Bernoulli and Binomial Random Variables
 *
 * - Keep the interface clean and transparent.
 * - Keep the implementation simple and to the point.
 * - Do not obfuscate user interaction.
 * - Encapsulate critical aspects of the interface.
 * - Hide complexity if and only if reasonably possible.
 */

#include "lehmer.h"

#include <math.h>
#include <stdio.h>
#include <time.h>

// Lehmer state management

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
        fprintf(stderr, " %u - 1\n", state->size);
    } else {
        fprintf(stderr, "\n");
    }
}

// Lehmer seed management

// @brief Select a generated seed from the current state in a given index
void lehmer_seed_select(lehmer_state_t* state, uint32_t index) {
    // Mitigate underflow and overflow
    state->size = (0 >= state->size) ? LEHMER_SIZE : state->size;
    // Bind the index to allocated range
    state->index = index % state->size;
}

int32_t lehmer_seed_get(lehmer_state_t* state) {
    // bind and fetch the selected seed
    lehmer_seed_select(state, state->index);
    return state->seed[state->index];
}

void lehmer_seed_set(lehmer_state_t* state, int32_t value) {
    // Bind the state and index for the value
    lehmer_seed_select(state, state->index);
    // Bind the value to the modulus
    state->seed[state->index] = (int32_t) (value % LEHMER_MODULUS);
}

void lehmer_seed_regenerate(
    lehmer_state_t* state, lehmer_generate_t generator
) {
    // Generate a new sequence based on the selected seed
    int32_t seed = lehmer_seed_get(state);
    lehmer_generate(state, generator, seed);
}

// Lehmer seed normalization

// @brief Normalizes a seed to a float in the range 0.0 to 1.0
static inline float lehmer_seed_normalize_to_float(lehmer_state_t* state) {
    lehmer_seed_select(state, state->index);
    return (float) state->seed[state->index] / (float) LEHMER_MODULUS;
}

// @brief Normalizes a seed to a integer in the range 0 to m - 1
static inline int32_t
lehmer_seed_normalize_to_int(int32_t value, uint32_t modulus) {
    return (int32_t) ((value + modulus) % modulus);
}

// Lehmer seed generation

void lehmer_generate_modulo(lehmer_state_t* state) {
    const uint32_t a = LEHMER_MULTIPLIER;
    const uint32_t m = LEHMER_MODULUS;

    uint32_t i = state->index;
    int32_t z = state->seed[i];

    int32_t r = (int32_t) (a * z) % m; // modulo
    state->seed[i] = lehmer_seed_normalize_to_int(r, m);
}

// Generates a sequence using gamma approach
void lehmer_generate_gamma(lehmer_state_t* state) {
    const uint32_t a = LEHMER_MULTIPLIER;
    const uint32_t m = LEHMER_MODULUS;
    const uint32_t q = LEHMER_QUOTIENT;
    const uint32_t r = LEHMER_REMAINDER;

    uint32_t i = state->index;
    int32_t z = state->seed[i];

    // @todo Validate the order of operations
    int32_t y = (int32_t) ((a * (z % q)) - (r * (z / q))); // gamma
    state->seed[i] = lehmer_seed_normalize_to_int(y, m);
}

// Generates a sequence using delta approach
void lehmer_generate_delta(lehmer_state_t* state) {
    const uint32_t a = LEHMER_MULTIPLIER;
    const uint32_t m = LEHMER_MODULUS;
    const uint32_t q = LEHMER_QUOTIENT;

    uint32_t i = state->index;
    int32_t z = state->seed[i];

    // @todo Validate the order of operations
    int32_t d = (int32_t) ((z / q) - (a * (z / m))); // delta
    state->seed[i] = lehmer_seed_normalize_to_int(d, m);
}

void lehmer_generate_jump(lehmer_state_t* state) {
    const uint32_t a = LEHMER_JUMP;
    const uint32_t m = LEHMER_MODULUS;
    const uint32_t q = LEHMER_QUOTIENT;
    const uint32_t r = LEHMER_REMAINDER;

    uint32_t i = state->index;
    int32_t z = state->seed[i];

    int32_t j = (int32_t) ((a * (z % q)) - (r * (z / q)));
    state->seed[i] = lehmer_seed_normalize_to_int(j, m);
}

// Generalized Lehmer sequence generator
void lehmer_generate(
    lehmer_state_t* state, lehmer_generate_t generator, int32_t seed
) {
    // Initialize the first seed
    state->seed[0] = seed;

    // Generate sequence of seeds
    for (uint32_t i = 1; i < state->size; i++) {
        state->index = i - 1;
        generator(state); // Apply the generation function
    }
}

// Generates a sequence using the current time as seed (non-deterministic)
void lehmer_generate_time(lehmer_state_t* state, lehmer_generate_t generator) {
    // Get the current time as seed
    time_t now = time(NULL);
    if (-1 == now) {
        lehmer_generate(state, generator, (int32_t) now);
    } else {
        lehmer_generate(state, generator, (int32_t) LEHMER_SEED);
    }
}

// Lehmer number generators

// modulo random number generation
float lehmer_random_modulo(lehmer_state_t* state) {
    lehmer_generate_modulo(state);
    return lehmer_seed_normalize_to_float(state);
}

// gamma random number generation
float lehmer_random_gamma(lehmer_state_t* state) {
    lehmer_generate_gamma(state);
    return lehmer_seed_normalize_to_float(state);
}

// delta random number generation
float lehmer_random_delta(lehmer_state_t* state) {
    lehmer_generate_delta(state);
    return lehmer_seed_normalize_to_float(state);
}

// Generalized random number generation
float lehmer_random(lehmer_state_t* state, lehmer_generate_t generator) {
    generator(state);
    return lehmer_seed_normalize_to_float(state);
}

// Lehmer probability functions

static inline bool lehmer_is_valid_probability(float p) {
    return (0.0 < p && p < 1.0);
}

int32_t lehmer_bernoulli(lehmer_state_t* state, float p) {
    if (!lehmer_is_valid_probability(p)) {
        // Return edge case (0 or 1) based on probability
        return (p <= 0.0) ? 0 : 1;
    }

    // Generate the Bernoulli outcome
    return (lehmer_random_modulo(state) < p) ? 1 : 0;
}

int32_t lehmer_binomial(lehmer_state_t* state, uint32_t n, float p) {
    int32_t count = 0;

    // Validate p and n
    if (!lehmer_is_valid_probability(p)) {
        return (p <= 0.0) ? 0 : n; // All failures or all successes
    }
    if (n == 0) {
        return 0; // No trials, no successes
    }

    // Sum up the results of n Bernoulli trials
    for (uint32_t i = 0; i < n; ++i) {
        count += lehmer_bernoulli(state, p);
    }

    return count;
}
