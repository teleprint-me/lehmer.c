/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/lehmer.c
 *
 * @brief A comprehensive implementation of the Lehmer RNG in pure C
 */

#include "lehmer.h"

#include <math.h>
#include <stdio.h>
#include <time.h>

// Create and initialize the state with dynamic stream handling
lehmer_state_t* lehmer_state_create(uint32_t size, int32_t value) {
    lehmer_state_t* state = (lehmer_state_t*) malloc(sizeof(lehmer_state_t));
    if (NULL == state) {
        return NULL;
    }

    state->seed = (int32_t*) malloc(sizeof(int32_t) * size);
    if (NULL == state->seed) {
        free(state);
        return NULL;
    }

    state->stream = 0; // select the first stream
    state->size = (0 == size) ? 1 : size; // coerce a size of 1

    // set the first seed
    state->seed[state->stream] = (int32_t) (value % LEHMER_MODULUS);
    if (0 > state->seed[state->stream]) {
        state->seed[state->stream] += LEHMER_MODULUS;
    }

    // Initialize remaining streams based on the first one
    for (uint32_t i = 1; i < state->size; i++) {
        int32_t z = state->seed[i - 1]; // previous seed
        state->seed[i] = (int32_t) ((LEHMER_MULTIPLIER * z) % LEHMER_MODULUS);
        if (state->seed[i] < 0) {
            state->seed[i] += LEHMER_MODULUS;
        }
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

void lehmer_state_print(lehmer_state_t* state) {
    fprintf(stderr, "lehmer->size: %zu\n", state->size);
    fprintf(stderr, "lehmer->stream: %zu\n", state->stream);
    fprintf(stderr, "lehmer->seed:");
    // print the first 10 seeds or size... whichever is less.
    uint32_t boundary = (state->size > 10) ? 10 : state->size;
    for (uint32_t i = 0; i < boundary; i++) {
        fprintf(stderr, " %lu,", state->seed[i]);
    }
    if (state->size > boundary) {
        fprintf(stderr, " ...\n");
    } else {
        fprintf(stderr, "\n");
    }
}

void lehmer_state_select(lehmer_state_t* state, uint32_t stream) {
    // Stream selection should not trigger seeding;
    // assume streams are initialized.
    state->stream = stream % state->size;
}

int32_t lehmer_seed_get(lehmer_state_t* state) {
    return state->seed[state->stream];
}

void lehmer_seed_set(lehmer_state_t* state, int32_t value) {
    // Ensure seed is within the modulus range
    state->seed[state->stream] = (int32_t) (value % LEHMER_MODULUS);
}

// @note: generates a number in the range 0.0 to 1.0
float lehmer_seed_normalize(lehmer_state_t* state) {
    return (float) state->seed[state->stream] / (float) LEHMER_MODULUS;
}

// Initialize the RNG state with seeds; decoupled from stream selection.
void lehmer_seed_streams(lehmer_state_t* state, int32_t value) {
    const int32_t q = LEHMER_MODULUS / LEHMER_JUMP;
    const int32_t r = LEHMER_MODULUS % LEHMER_JUMP;
    const uint32_t s = state->stream; // backup

    // Select the first stream
    lehmer_state_select(state, 0);
    // Seed the first stream
    lehmer_seed_set(state, value);

    state->stream = s; // restore

    // Initialize remaining streams based on the first one
    for (uint32_t i = 1; i < state->size; i++) {
        int32_t z = state->seed[i - 1]; // previous seed
        state->seed[i] = (int32_t) (((LEHMER_JUMP * z) % q) - ((r * z) / q));
    }
}

// @note: the lehmer_generate_* functions generate the next seed

void lehmer_generate_modulo(lehmer_state_t* state) {
    int32_t z = state->seed[state->stream];
    int64_t a = LEHMER_MULTIPLIER;
    int64_t m = LEHMER_MODULUS;
    int64_t r = (int64_t) (a * z) % m; // remainder
    int32_t o = (r > 0) ? (int32_t) r : (int32_t) (r + m); // output
    state->seed[state->stream] = o;
}

void lehmer_generate_gamma(lehmer_state_t* state) {
    int32_t z = state->seed[state->stream];
    uint64_t a = LEHMER_MULTIPLIER;
    uint64_t m = LEHMER_MODULUS;
    uint64_t q = m / a; // quotient
    uint64_t r = m % a; // remainder
    int32_t y = (int32_t) (((a * z) % q) - ((r * z) / q)); // gamma
    int32_t o = (y > 0) ? (int32_t) y : (int32_t) (y + m); // output
    state->seed[state->stream] = o;
}

void lehmer_generate_delta(lehmer_state_t* state) {
    int32_t z = state->seed[state->stream];
    uint64_t a = LEHMER_MULTIPLIER;
    uint64_t m = LEHMER_MODULUS;
    int32_t q = m / a; // quotient
    int32_t d = (int32_t) ((z / q) - ((a * z) / m)); // delta
    int32_t o = (d > 0) ? (int32_t) d : (int32_t) (d + m); // output
    state->seed[state->stream] = o;
}

// @note: the lehmer_random_* functions generate the next random value

// modulo random number generation
float lehmer_random_modulo(lehmer_state_t* state) {
    lehmer_generate_modulo(state);
    return lehmer_seed_normalize(state);
}

// gamma random number generation
float lehmer_random_gamma(lehmer_state_t* state) {
    lehmer_generate_gamma(state);
    return lehmer_seed_normalize(state);
}

// delta random number generation
float lehmer_random_delta(lehmer_state_t* state) {
    lehmer_generate_delta(state);
    return lehmer_seed_normalize(state);
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
int32_t lehmer_bernoulli(lehmer_state_t* state, float p) {
    // Ensure p is within the valid range
    if (p <= 0.0) {
        return 0; // Always fail
    }
    if (p >= 1.0) {
        return 1; // Always succeed
    }

    // Generate the Bernoulli outcome
    return ((lehmer_random_gamma(state) < p) ? 1 : 0);
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
int32_t lehmer_binomial(lehmer_state_t* state, uint32_t n, float p) {
    int32_t count = 0;

    // Validate p and n
    if (p <= 0.0) {
        return 0; // No successes
    }
    if (p >= 1.0) {
        return n; // All successes
    }
    if (n == 0) {
        return 0; // No trials
    }

    // Sum up the results of n Bernoulli trials
    for (uint32_t i = 0; i < n; ++i) {
        count += lehmer_bernoulli(state, p);
    }

    return count;
}
