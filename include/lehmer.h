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
 * https://math.libretexts.org/Bookshelves/Combinatorics_and_Discrete_Mathematics/Discrete_Mathematics_(Levin)
 * - Reference 1.2 Binomial Coefficients for definitions
 * - Reference 5.1 Generating Functions for details
 */

#ifndef LEHMER_H
#define LEHMER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

// These constants were defined with 32-bits in mind, not 64-bits.
#define MODULUS    2147483647 // Mersenne prime number used as modulus (2^31 - 1)
#define MULTIPLIER 48271      // Multiplier for the Lehmer RNG
#define CHECK      399268537 // Check value for correctness
#define STREAMS    256       // Number of streams
#define A256       22925     // Jump multiplier for stream separation
#define DEFAULT    123456789 // Default seed value

typedef struct LehmerState {
    uint64_t* seed;        // Current state of each stream
    size_t    stream;      // Current stream index
    size_t    size;        // Number of streams
    bool      initialized; // True if RNG is initialized
} lehmer_state_t;

lehmer_state_t* lehmer_create_state(size_t size);
void            lehmer_free_state(lehmer_state_t* state);

void     lehmer_set_seed(lehmer_state_t* state, uint64_t value);
uint64_t lehmer_get_seed(lehmer_state_t* state);

void lehmer_select_stream(lehmer_state_t* state, size_t stream);
void lehmer_seed_streams(lehmer_state_t* state, uint64_t value);

double lehmer_generate(lehmer_state_t* state);

#endif // LEHMER_H
