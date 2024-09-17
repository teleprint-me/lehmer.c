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
 * m: A large prime number
 *
 * @note Must be a mersenne prime number
 * - 32-bit: 2^31 - 1 = 2147483647
 * - 64-bit: 2^61 - 1 = 2305843009213693951
 *
 * @note Originally chosen for the IBM System/360 to avoid overflow errors
 *       in 32-bit integer arithmetic. This choice remains relevant for
 *       modern embedded systems with similar constraints.
 */
#define LEHMER_MODULUS 2147483647

/**
 * @param LEHMER_MULTIPLIER Prime number used to scale the seed.
 *
 * a: A smaller prime number in the range 2, ..., m - 1
 *
 * @note Must be a prime number
 *
 * @note The multiplier was specifically selected to prevent overflow errors
 *       on hardware with 32-bit integer limitations. Despite being dated,
 *       this choice is still effective in modern contexts where 32-bit
 *       constraints apply.
 *
 * @warning 16807 was originally chosen for safe 32-bit arithmetic
 */
#define LEHMER_MULTIPLIER 48271

/**
 * @param LEHMER_JUMP Prime number used as a multiplier to "jump" scale seeds.
 *
 * @note The multiplier must be a prime number.
 */
#define LEHMER_JUMP 22937

/**
 * @param LEHMER_SIZE Default allocation size for a given number of seeds
 */
#define LEHMER_SIZE 256

/**
 * @param LEHMER_SEED Default seed value
 *
 * z: A value between 0, ..., m - 1
 *
 * @note May be any value within the defined range
 */
#define LEHMER_SEED 123456789

/**
 * @param LEHMER_QUOTIENT Result of dividing LEHMER_MODULUS by
 * LEHMER_MULTIPLIER. Useful for range normalization in Lehmer-based
 * calculations.
 */
#define LEHMER_QUOTIENT ((LEHMER_MODULUS) / (LEHMER_MULTIPLIER))

/**
 * @param LEHMER_REMAINDER Result of modulus operation between LEHMER_MODULUS
 * and LEHMER_MULTIPLIER. Important for handling overflows and adjusting
 * outputs.
 */
#define LEHMER_REMAINDER ((LEHMER_MODULUS) % (LEHMER_MULTIPLIER))

/**
 * @brief Structure representing the state of the LCG RNG
 *
 * @param seed Array of seeds representing the initial state
 * @param size The number of seeds (upper limit)
 * @param index The selected seed to generate sequences from
 */
typedef struct LehmerState {
    int32_t* seed; // Array of seeds representing the initial state
    uint32_t size; // The number of seeds (upper limit)
    uint32_t index; // The selected seed to generate sequences from
} lehmer_state_t;

/**
 * @note useful for autocomplete and mnemonic (easy to remember or ref)
 *
 * (prefix)_(group)_(v.|adj.|n.)
 *
 * prefix: lehmer, group: state, verb: create
 * -> lehmer_state_create
 */

typedef void (*lehmer_generate_t)(lehmer_state_t*);

// Lehmer state management

lehmer_state_t* lehmer_state_create(uint32_t size, int32_t seed);
void lehmer_state_free(lehmer_state_t* state);
void lehmer_state_reseed(
    lehmer_state_t* state, lehmer_generate_t generate, uint32_t index
);
void lehmer_state_reseed(
    lehmer_state_t* state, lehmer_generate_t generate, uint32_t index
);
void lehmer_state_select(lehmer_state_t* state, uint32_t index);
void lehmer_state_print(lehmer_state_t* state);

// Seed seed management

int32_t lehmer_seed_bind(int32_t value, uint32_t modulus);
int32_t lehmer_seed_get(lehmer_state_t* state);
void lehmer_seed_set(
    lehmer_state_t* state, lehmer_generate_t generate, int32_t value
);
float lehmer_seed_normalize(lehmer_state_t* state);
void lehmer_seed_streams(lehmer_state_t* state, int32_t value);

// Generate related functions

// @note lehmer_generate_* functions generate new seeds

/**
 * @brief The Lehmer Random Number Generator is a simple yet elegant method
 * based on the equation f(z) = az \mod m.
 *
 * The core of Lehmer’s RNG is defined by the iterative equation
 *
 * z_{n+1} = \f(z_n), where \f(z) = a \cdot z \mod m.
 *
 * @param[in] state The current state object.
 */
void lehmer_generate_modulo(lehmer_state_t* state);

/**
 * @note Intermediate results are bounded by m - 1
 * \f(z) = \gamma(z) + m \cdot \delta(z)
 */

/**
 * @brief Implementation of the gamma function for the Lehmer LCG PRNG
 *
 * This function calculates a scaled and shifted version of the original Lehmer
 * sequence, adjusting the range. It is an essential part of the binding
 * function for the Lehmer LCG PRNG.
 *
 * \gamma(z) = a \cdot (z \mod q) - r \cdot (z \div q)
 *
 * @param[in] state The current state object.
 */
void lehmer_generate_gamma(lehmer_state_t* state);

/**
 * @brief Implementation of the delta function for the Lehmer LCG PRNG
 *
 * This function calculates the delta value for the Lehmer LCG PRNG, which
 * might serve as an error correction term for the seed produced by the gamma
 * function. The implementation is based on the hypothesis that delta depends
 * on the same variables as gamma and is computed in a way that tries to
 * minimize the error introduced in the seed.
 *
 * \delta(z) = (z \div q) - (a \cdot z \div m)
 *
 * @param[in] state The current state object.
 */
void lehmer_generate_delta(lehmer_state_t* state);

void lehmer_generate_jump(lehmer_state_t* state);
void lehmer_generate(
    lehmer_state_t* state, lehmer_generate_t generate, int32_t seed
);
void lehmer_generate_time(lehmer_state_t* state, lehmer_generate_t generate);

// Randome related functions

// @note lehmer_random_* functions generate normalized random numbers
float lehmer_random_modulo(lehmer_state_t* state);
float lehmer_random_gamma(lehmer_state_t* state);
float lehmer_random_delta(lehmer_state_t* state);
float lehmer_random(lehmer_state_t* state, lehmer_generate_t generate);

// Variate related functions

/**
 * @todo variates are a work in progress
 *
 * @note new issue arises because each of the variates requires a generator as
 * well
 */

int32_t lehmer_bernoulli(lehmer_state_t* state, float p);
int32_t lehmer_binomial(lehmer_state_t* state, uint32_t n, float p);

#endif // LEHMER_H
