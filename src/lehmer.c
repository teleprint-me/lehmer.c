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
lehmer_state_t* lehmer_state_create(int32_t seed, uint32_t length) {
    // Allocate memory for managing the LCG PRNG state
    lehmer_state_t* state = (lehmer_state_t*) malloc(sizeof(lehmer_state_t));
    if (NULL == state) {
        return NULL;
    }

    // Zero-initialize the index
    state->position = 0;
    // Default to a seed of 123456789 if seed is 0
    lehmer_set_initial_seed(state, seed || LEHMER_SEED);
    // Default to a size of 256 if size is 0
    state->length = (0 >= length) ? LEHMER_SIZE : length;

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

// Lehmer seed management

// Set the initial seed with boundary enforcement (modulus operation)
void lehmer_set_initial_seed(lehmer_state_t* state, int32_t seed) {
    // Bind the seed to the global modulus
    state->seed = seed % LEHMER_MODULUS;
}

// Get the inital seed from the current state
int32_t lehmer_get_initial_seed(lehmer_state_t* state) {
    // Bind the seed to the global modulus
    return state->seed % LEHMER_MODULUS;
}

// Set the previous seed in the generated sequence
void lehmer_set_previous_seed(lehmer_state_t* state) {
    // Bind the position to the sequence length
    state->position = (state->position - 1) % state->length;
}

// Set the next seed in the generated sequence
void lehmer_set_next_seed(lehmer_state_t* state) {
    // Bind the position to the sequence length
    state->position = (state->position + 1) % state->length;
}

// Get the current seed in the sequence with boundary enforcement
int32_t lehmer_get_current_seed(lehmer_state_t* state) {
    // Bind the position to the sequence length
    state->position %= state->length;
    // Get the current seed from the sequence
    return state->sequence[state->position];
}

// Get the current seed in the sequence with boundary enforcement
int32_t lehmer_set_next_and_get_seed(lehmer_state_t* state) {
    // Set the next seed
    lehmer_set_next_seed(state);
    // Get the current seed from the sequence
    return lehmer_get_current_seed(state);
}

// Lehmer seed normalization

// @brief Normalizes a seed to a float in the range 0.0 to 1.0
float lehmer_seed_normalize_to_float(int32_t seed) {
    return (float) seed / (float) LEHMER_MODULUS;
}

// @brief Normalizes a seed to a integer in the range 0 to m - 1
int32_t lehmer_seed_normalize_to_int(int32_t seed) {
    // Bind the seed to the modulus
    return seed % LEHMER_MODULUS;
}

// Lehmer seed calculators

// Helper function to calculate the modulo formula
int32_t lehmer_calculate_modulo(int32_t seed, int32_t multiplier) {
    // Lehmer formula: (a * z) % m
    return (int32_t) (((int64_t) multiplier * seed) % LEHMER_MODULUS);
}

// Helper function to calculate the gamma formula
int32_t lehmer_calculate_gamma(int32_t seed, int32_t a, int32_t q, int32_t r) {
    // Gamma formula: (a * (z % q)) - (r * (z / q))
    int64_t scale_mod = (int64_t) (a * (seed % q));
    int64_t scale_quo = (int64_t) (r * (seed / q));
    return (int32_t) (scale_mod - scale_quo);
}

// Helper function to calculate the delta formula
int32_t lehmer_calculate_delta(int32_t seed, int32_t a, int32_t q, int32_t r) {
    // Delta formula: (z / q) - (a * (z / m))
    int64_t norm_seed = (int64_t) seed / LEHMER_QUOTIENT;
    int64_t scale_mul = (int64_t) LEHMER_MULTIPLIER * (seed / LEHMER_MODULUS);
    int32_t d = (int32_t) (norm_seed - scale_mul); // Delta operation
}

// Lehmer seed generators

// Generate the seed using modulo formula
int32_t lehmer_generate_modulo(int32_t seed) {
    int32_t r = lehmer_calculate_modulo(seed, LEHMER_MULTIPLIER);
    return lehmer_seed_normalize_to_int(r);
}

int32_t lehmer_generate_gamma(int32_t seed) {
    int32_t y = lehmer_calculate_gamma(
        seed, LEHMER_MULTIPLIER, LEHMER_QUOTIENT, LEHMER_REMAINDER
    );
    return lehmer_seed_normalize_to_int(y);
}

int32_t lehmer_generate_jump(int32_t seed) {
    int32_t j = lehmer_calculate_gamma(
        seed, LEHMER_JUMP, LEHMER_QUOTIENT, LEHMER_REMAINDER
    );
    return lehmer_seed_normalize_to_int(j);
}

int32_t lehmer_generate_delta(int32_t seed) {
    int32_t d = lehmer_calculate_delta(
        seed, LEHMER_JUMP, LEHMER_QUOTIENT, LEHMER_REMAINDER
    );
    return lehmer_seed_normalize_to_int(d);
}

// Lehmer seed generators

// Generalized Lehmer sequence generator
void lehmer_generate(
    lehmer_state_t* state, lehmer_generate_t generator, int32_t seed
) {
    // Set the initial seed within the range of the modulus
    state->seed = seed % LEHMER_MODULUS;

    // Generate the first value in the sequence based on the initial seed
    state->sequence[0] = generator(state->seed);

    // Generate and store the rest of the sequence
    for (uint32_t current = 1; current < state->length; current++) {
        // Calculate the previous index (ensures we reference the correct seed)
        uint32_t previous = current - 1;

        // Use the previous sequence value as the seed for the next
        int32_t previous_seed = state->sequence[previous] % LEHMER_MODULUS;

        // Generate the next value and store it in the current position
        state->sequence[current] = generator(previous_seed);
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

void lehmer_regenerate(lehmer_state_t* state, lehmer_generate_t generator) {
    // Get the selected seed in the sequence
    int32_t seed = lehmer_set_next_and_get_seed(state);
    // Generate a new sequence based on the selected seed
    lehmer_generate(state, generator, seed);
}

// Lehmer number generators

// Generate a random number using the modulo approach.
float lehmer_random_modulo(lehmer_state_t* state) {
    // Get the selected seed in the sequence
    int32_t seed = lehmer_set_next_and_get_seed(state);
    // Generate the next value using modulo
    seed = lehmer_generate_modulo(seed);
    // Normalize and return
    return lehmer_seed_normalize_to_float(seed);
}

// Generate a random number using the gamma approach.
float lehmer_random_gamma(lehmer_state_t* state) {
    // Get the selected seed in the sequence
    int32_t seed = lehmer_set_next_and_get_seed(state);
    seed = lehmer_generate_gamma(seed);
    return lehmer_seed_normalize_to_float(seed);
}

// Generate a random number using the jump approach.
float lehmer_random_jump(lehmer_state_t* state) {
    // Get the selected seed in the sequence
    int32_t seed = lehmer_set_next_and_get_seed(state);
    seed = lehmer_generate_jump(seed);
    return lehmer_seed_normalize_to_float(seed);
}

// Generate a random number using the modulo approach.
float lehmer_random_delta(lehmer_state_t* state) {
    // Get the selected seed in the sequence
    int32_t seed = lehmer_set_next_and_get_seed(state);
    seed = lehmer_generate_delta(seed);
    return lehmer_seed_normalize_to_float(seed);
}

// Generalized random number generation
float lehmer_random(lehmer_state_t* state, lehmer_generate_t generator) {
    // Get the selected seed in the sequence
    int32_t seed = lehmer_set_next_and_get_seed(state);
    seed = generator(seed);
    return lehmer_seed_normalize_to_float(seed);
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
