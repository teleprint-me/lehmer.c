/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/lehmer.c
 *
 * @brief A simple implementation of the Lehmer RNG in pure C
 */

#include "lehmer.h"

#include <math.h>
#include <time.h>

// Create and initialize the state with dynamic stream handling
lehmer_state_t* lehmer_state_create(size_t size, int64_t value) {
    lehmer_state_t* state = (lehmer_state_t*) malloc(sizeof(lehmer_state_t));
    if (NULL == state) {
        return NULL;
    }

    state->seed = (int64_t*) malloc(sizeof(int64_t) * size);
    if (NULL == state->seed) {
        free(state);
        return NULL;
    }

    state->stream = 0;                      // select the first stream
    state->size   = (0 == size) ? 1 : size; // coerce a size of 1

    // get the quotient and remainder
    const int64_t q = LEHMER_MODULUS / LEHMER_JUMP;
    const int64_t r = LEHMER_MODULUS % LEHMER_JUMP;

    // set the first seed
    state->seed[state->stream] = (int64_t) (value % LEHMER_MODULUS);

    // Initialize remaining streams based on the first one
    for (size_t i = 1; i < state->size; i++) {
        int64_t z      = state->seed[i - 1]; // previous seed
        state->seed[i] = (int64_t) ((LEHMER_JUMP * (z % q)) - (r * (z / q)));
    }

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

// Stream selection should not trigger seeding; assume streams are initialized.
void lehmer_state_select(lehmer_state_t* state, size_t stream) {
    state->stream = stream % state->size;
}

int64_t lehmer_seed_get(lehmer_state_t* state) {
    return state->seed[state->stream];
}

void lehmer_seed_set(lehmer_state_t* state, int64_t value) {
    // Ensure seed is within the modulus range
    state->seed[state->stream] = (int64_t) (value % LEHMER_MODULUS);
}

// @note: generates a number in the range 0.0 to 1.0
double lehmer_seed_normalize(lehmer_state_t* state) {
    return (double) state->seed[state->stream] / (double) LEHMER_MODULUS;
}

// Initialize the RNG state with seeds; decoupled from stream selection.
void lehmer_seed_streams(lehmer_state_t* state, int64_t value) {
    const int64_t q = LEHMER_MODULUS / LEHMER_JUMP;
    const int64_t r = LEHMER_MODULUS % LEHMER_JUMP;
    const size_t  s = state->stream; // backup

    // Select the first stream
    lehmer_state_select(state, 0);
    // Seed the first stream
    lehmer_seed_set(state, value);

    state->stream = s; // restore

    // Initialize remaining streams based on the first one
    for (size_t i = 1; i < state->size; i++) {
        int64_t z      = state->seed[i - 1]; // previous seed
        state->seed[i] = (int64_t) ((LEHMER_JUMP * (z % q)) - (r * (z / q)));
    }
}

// Generate the next random number
double lehmer_generate(lehmer_state_t* state) {
    const int64_t quotient     = MODULUS / MULTIPLIER;
    const int64_t remainder    = MODULUS % MULTIPLIER;
    const int64_t current_seed = state->seed[state->stream];
    // Explicitly typecast signed results
    const int64_t new_seed
        = (int64_t) (MULTIPLIER * (current_seed % quotient)
                     - remainder * (current_seed / quotient));

    state->seed[state->stream] = new_seed;

    return ((double) new_seed / MODULUS);
}

/**
 * @brief Returns 1 with probability p or 0 with probability 1 - p.
 * NOTE: use 0.0 < p < 1.0
 *
 * A random variable X whose probability mass function is given is called a
 * Bernoulli Random Variable with parameter p and we write X ~ Bern(p).
 *
 * f(x) = P(X = x) = {
 *     1 - p if x = 0,
 *     p if x = 1,
 *     0 otherwise
 * } = {
 *     (p^x)((1 - p)^(1-x)) if x = (0, 1)
 *     0 otherwise
 * }
 */
int lehmer_bernoulli(lehmer_state_t* state, double p) {
    // p is probability of success
    // q is probability of failure (1 - p)
    // The probability of success and failure must sum to 1
    // each trial must always end with a success or failure
    //     p + q = 1
    // returns 0 on success, 1 on failure
    return ((lehmer_generate(state) < (1.0 - p)) ? 0 : 1);
}

/**
 * @brief Returns a binomial distributed integer between 0 and n inclusive.
 * @note use n > 0 and 0.0 < p < 1.0
 *
 * For each integer n ≥ 0 and integer k with 0 ≤ k ≤ n there is a number (n k).
 * A random variable X whose probability mass function is given is called a
 * Binomial random variable with parameters n and p.
 *
 * f(x) = P(X = x) = {
 *     (n x) p^x (1 - p)^(n - x) if x = 0, 1, 2, ..., n
 *     0 otherwise
 * }
 */
int lehmer_binomial(lehmer_state_t* state, size_t n, double p) {
    long i, x = 0;

    for (size_t i = 0; i < n; i++) {
        x += lehmer_bernoulli(state, p);
    }

    return (x);
}
