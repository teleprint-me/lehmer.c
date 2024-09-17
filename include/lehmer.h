/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/lehmer.h
 *
 * @brief A comprehensive implementation of the Lehmer RNG in pure C
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
 * @param LEHMER_MODULUS Mersenne prime number used as modulus (2^31 - 1).
 *
 * @note Originally chosen for the IBM System/360 to avoid overflow errors
 *       in 32-bit integer arithmetic. This choice remains relevant for
 *       modern embedded systems with similar constraints.
 */
#define LEHMER_MODULUS 2147483647

/**
 * @param LEHMER_MULTIPLIER Multiplier used to scale the seed
 *
 * @note The multiplier must be a prime number.
 *
 * @note The multiplier was specifically selected to prevent overflow errors
 *       on hardware with 32-bit integer limitations. Despite being dated,
 *       this choice is still effective in modern contexts where 32-bit
 *       constraints apply.
 */
#define LEHMER_MULTIPLIER 48271

/**
 * @param LEHMER_JUMP Multiplier for "jump" scaling seeds
 *
 * @note The multiplier must be a prime number.
 */
#define LEHMER_JUMP 22937

/**
 * @param LEHMER_STREAMS Number of streams
 */
#define LEHMER_STREAMS 256

/**
 * @param LEHMER_SEED Default seed value
 */
#define LEHMER_SEED 123456789

/**
 * @param LEHMER_CHECK_SEED Used in testing for validation
 */
#define LEHMER_CHECK_SEED 1753928844 // After 10,000 iterations

/**
 * @param LEHMER_CHECK_VALUE Used in testing for validation
 */
#define LEHMER_CHECK_VALUE 0.816736763 // After 10,000 iterations

/**
 * @param LEHMER_CHECK_JUMP Used in testing for validation
 */
#define LEHMER_CHECK_JUMP 399268537

/**
 * @brief Structure representing the state of the LCG RNG
 *
 * @param seed Pointer to the current state of each stream
 * @param stream Number representing the current stream index
 * @param size Number of streams
 */
typedef struct LehmerState {
    int32_t* seed;
    uint32_t stream;
    uint32_t size;
} lehmer_state_t;

// @note useful for autocomplete and mnemonic (easy to remember or ref)
// prefix_group_verb|adj.|pro[noun]
// prefix: lehmer, group: state, verb: create
// -> lehmer_state_create

lehmer_state_t* lehmer_state_create(uint32_t size, int32_t value);
void lehmer_state_free(lehmer_state_t* state);
void lehmer_state_select(lehmer_state_t* state, uint32_t stream);
void lehmer_state_print(lehmer_state_t* state);

int32_t lehmer_seed_get(lehmer_state_t* state);
void lehmer_seed_set(lehmer_state_t* state, int32_t value);
float lehmer_seed_normalize(lehmer_state_t* state);
void lehmer_seed_streams(lehmer_state_t* state, int32_t value);

// @note lehmer_generate_* functions generate new seeds
void lehmer_generate_modulo(lehmer_state_t* state);
void lehmer_generate_gamma(lehmer_state_t* state);
void lehmer_generate_delta(lehmer_state_t* state);

// @note lehmer_random_* functions generate normalized random numbers
float lehmer_random_modulo(lehmer_state_t* state);
float lehmer_random_gamma(lehmer_state_t* state);
float lehmer_random_delta(lehmer_state_t* state);

/* @todo variates are a work in progress */
int32_t lehmer_bernoulli(lehmer_state_t* state, float p);
int32_t lehmer_binomial(lehmer_state_t* state, uint32_t n, float p);

#endif // LEHMER_H
