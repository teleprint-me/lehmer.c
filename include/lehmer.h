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
 * Discrete Mathematics
 * - 1.2 Binomial Coefficients for definitions
 * - 5.1 Generating Functions for details
 * https://math.libretexts.org/Bookshelves/Combinatorics_and_Discrete_Mathematics/Discrete_Mathematics_(Levin)
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
 * @param CHECK Used in testing for validation
 */
#define CHECK      399268537

/**
 * @param DEFAULT Default seed value
 */
#define DEFAULT    123456789

/**
 * @brief Structure representing the state of the LCG RNG
 *
 * @param seed Pointer to the current state of each stream
 * @param stream Number representing the current stream index
 * @param size Number of streams
 * @param initialized True if RNG is initialized
 */
typedef struct LehmerState {
    uint64_t* seed;
    size_t    stream;
    size_t    size;
    bool      initialized;
} lehmer_state_t;

lehmer_state_t* lehmer_create_state(size_t size);
void            lehmer_free_state(lehmer_state_t* state);

void     lehmer_set_seed(lehmer_state_t* state, uint64_t value);
uint64_t lehmer_get_seed(lehmer_state_t* state);

void lehmer_select_stream(lehmer_state_t* state, size_t stream);
void lehmer_seed_streams(lehmer_state_t* state, uint64_t value);

double lehmer_generate(lehmer_state_t* state);

int bernoulli(lehmer_state_t* state, double p);

#endif // LEHMER_H
