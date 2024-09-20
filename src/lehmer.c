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
    lehmer_seed_set(state, seed);
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
void lehmer_seed_set(lehmer_state_t* state, int32_t seed) {
    state->seed = seed % LEHMER_MODULUS;
}

// Retrieve the current seed from the state
int32_t lehmer_seed_get(lehmer_state_t* state) {
    return state->seed;
}

// Lehmer position management

/**
 * @brief Set the current position in the sequence (handles overflow with
 * modulus).
 *
 * @param state Pointer to the Lehmer state structure.
 * @param position New position to set.
 */
void lehmer_position_set(lehmer_state_t* state, uint32_t position) {
    // Ensure position stays within bounds
    state->position = position % state->length;
}

/**
 * @brief Move to the next position in the sequence (handles overflow).
 *
 * @param state Pointer to the Lehmer state structure.
 */
void lehmer_position_next(lehmer_state_t* state) {
    // Increment and wrap around
    lehmer_position_set(state, (state->position + 1));
}

/**
 * @brief Move to the previous position in the sequence (handles underflow).
 *
 * @param state Pointer to the Lehmer state structure.
 */
void lehmer_position_previous(lehmer_state_t* state) {
    // Decrement and wrap around
    lehmer_position_set(state, (state->position - 1));
}

// Lehmer sequence management

// Set a value in the sequence with boundary enforcement
void lehmer_sequence_set(lehmer_state_t* state, int32_t seed) {
    state->sequence[state->position] = seed % LEHMER_MODULUS;
}

// Get the current value in the sequence
int32_t lehmer_sequence_get(lehmer_state_t* state) {
    return state->sequence[state->position];
}

// Lehmer seed normalization

// @brief Normalizes a seed to a float in the range 0.0 to 1.0
float lehmer_seed_normalize_to_float(int32_t seed) {
    return (float) seed / (float) LEHMER_MODULUS;
}

// @brief Normalizes a seed to a integer in the range 0 to m - 1
int32_t lehmer_seed_normalize_to_int(int32_t seed, uint32_t modulus) {
    return (seed + modulus) % modulus;
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
    return lehmer_seed_normalize_to_int(r, LEHMER_MODULUS);
}

int32_t lehmer_generate_gamma(int32_t seed) {
    int32_t y = lehmer_calculate_gamma(
        seed, LEHMER_MULTIPLIER, LEHMER_QUOTIENT, LEHMER_REMAINDER
    );
    return lehmer_seed_normalize_to_int(y, LEHMER_MODULUS);
}

int32_t lehmer_generate_jump(int32_t seed) {
    int32_t j = lehmer_calculate_gamma(
        seed, LEHMER_JUMP, LEHMER_QUOTIENT, LEHMER_REMAINDER
    );
    return lehmer_seed_normalize_to_int(j, LEHMER_MODULUS);
}

int32_t lehmer_generate_delta(int32_t seed) {
    int32_t d = lehmer_calculate_delta(
        seed, LEHMER_JUMP, LEHMER_QUOTIENT, LEHMER_REMAINDER
    );
    return lehmer_seed_normalize_to_int(d, LEHMER_MODULUS);
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
    // Generate a new sequence based on the selected seed
    int32_t seed = lehmer_seed_get(state);
    lehmer_generate(state, generator, seed);
}

// Lehmer number generators

// Generate a random number using the modulo approach.
float lehmer_random_modulo(lehmer_state_t* state) {
    // Move to the next position in the sequence
    lehmer_position_next(state);
    // Get the current seed
    int32_t seed = lehmer_sequence_get(state);
    // Generate the next value using modulo
    seed = lehmer_generate_modulo(seed);
    // Normalize and return
    return lehmer_seed_normalize_to_float(seed);
}

// Generate a random number using the gamma approach.
float lehmer_random_gamma(lehmer_state_t* state) {
    lehmer_position_next(state);
    int32_t seed = lehmer_sequence_get(state);
    seed = lehmer_generate_gamma(seed);
    return lehmer_seed_normalize_to_float(seed);
}

// Generate a random number using the jump approach.
float lehmer_random_jump(lehmer_state_t* state) {
    lehmer_position_next(state);
    int32_t seed = lehmer_sequence_get(state);
    seed = lehmer_generate_jump(seed);
    return lehmer_seed_normalize_to_float(seed);
}

// Generate a random number using the modulo approach.
float lehmer_random_delta(lehmer_state_t* state) {
    lehmer_position_next(state);
    int32_t seed = lehmer_sequence_get(state);
    seed = lehmer_generate_delta(seed);
    return lehmer_seed_normalize_to_float(seed);
}

// Generalized random number generation
float lehmer_random(lehmer_state_t* state, lehmer_generate_t generator) {
    lehmer_position_next(state);
    int32_t seed = lehmer_sequence_get(state);
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
