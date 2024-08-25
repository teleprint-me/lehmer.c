/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/lehmer.h
 *
 * @brief A simple implementation of the Lehmer RNG in pure C
 *
 * Title: Random number generators: good ones are hard to find
 * Paper: https://dl.acm.org/doi/10.1145/63039.63042
 * Source: https://www.cs.wm.edu/~va/software/park/park.html
 *
 * If you get stuck, see the following for more details:
 *
 *   Discrete Mathematics
 *     - 1.2 Binomial Coefficients for definitions
 *     - 5.1 Generating Functions for details
 *
 *   Introduction to Probability and Mathematical Statistics
 *     - 9.3: The Bernoulli and Binomial Random Variables
 */

#ifndef LEHMER_H
#define LEHMER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @param MODULUS Mersenne prime number used as modulus (2^31 - 1)
 *
 * @note Originally chosen for the IBM System/360 to avoid overflow errors
 *       in 32-bit integer arithmetic. This choice remains relevant for
 *       modern embedded systems with similar constraints.
 */
#define MODULUS    2147483647

/**
 * @param MULTIPLIER Chosen for safe 32-bit arithmetic (16807)
 *
 * @note The multiplier was specifically selected to prevent overflow errors
 *       on hardware with 32-bit integer limitations. Despite being dated,
 *       this choice is still effective in modern contexts where 32-bit
 *       constraints apply.
 */
#define MULTIPLIER 48271

/**
 * @param A256 Jump multiplier for stream separation
 */
#define A256       22925

/**
 * @param STREAMS Number of streams
 */
#define STREAMS    256

/**
 * @param DEFAULT Default seed value
 */
#define DEFAULT    123456789

/**
 * @param CHECK Used in testing for validation
 */
#define CHECK      399268537

/**
 * @brief Structure representing the state of the LCG RNG
 *
 * @param seed Pointer to the current state of each stream
 * @param stream Number representing the current stream index
 * @param length Number of streams
 * @param initialized True if RNG is initialized
 */
typedef struct LehmerState {
    uint64_t* seed;
    size_t    stream;
    size_t    length;
    bool      initialized;
} lehmer_state_t;

// @note useful for autocomplete and mnemonic (easy to remember or ref)
// prefix_group_verb|adj.|pro[noun]
// prefix: lehmer, group: state, verb: create
// -> lehmer_state_create

lehmer_state_t* lehmer_state_create(size_t size);
void            lehmer_state_free(lehmer_state_t* state);
void            lehmer_state_select(lehmer_state_t* state, size_t stream);

uint64_t lehmer_seed_get(lehmer_state_t* state);
void     lehmer_seed_set(lehmer_state_t* state, uint64_t value);
double   lehmer_seed_normalize(lehmer_state_t* state);
void     lehmer_seed_streams(lehmer_state_t* state, uint64_t value);

// @note lehmer_generate_* functions generate new seeds
uint64_t lehmer_generate_modulo(lehmer_state_t* state);
uint64_t lehmer_generate_gamma(lehmer_state_t* state);
uint64_t lehmer_generate_delta(lehmer_state_t* state);

// @note lehmer_random_* functions generate normalized random numbers
double lehmer_random_modulo(lehmer_state_t* state);
double lehmer_random_gamma(lehmer_state_t* state);
double lehmer_random_delta(lehmer_state_t* state);

/* @todo variates are a work in progress */
int lehmer_bernoulli(lehmer_state_t* state, double p);
int lehmer_binomial(lehmer_state_t* state, double p, size_t n);

#endif // LEHMER_H
