/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/concept.c
 *
 * @brief Start from scratch
 */

#include <stddef.h>
#include <stdint.h>
#include <time.h>

/**
 * @param LEHMER_MODULUS Mersenne prime number used as modulus (2^31 - 1).
 */
#define LEHMER_MODULUS 2147483647

/**
 * @param LEHMER_MULTIPLIER Prime number used to scale the seed.
 */
#define LEHMER_MULTIPLIER 48271

/**
 * @param LEHMER_JUMP Prime number used as a multiplier to "jump" scale seeds.
 */
#define LEHMER_JUMP 22937

/**
 * @param LEHMER_SIZE Default allocation size for a given number of seeds
 */
#define LEHMER_SIZE 256

/**
 * @param LEHMER_SEED Default seed value
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

// @note the size of seed and stream must be the same
// the general idea is that we select a seed
// then generate a new sequence from the selected seed
typedef struct LehmerState {
    int32_t* seed; // Array of seeds representing the initial state
    uint32_t index; // The selected seed to generate sequences from
    uint32_t size; // The number of seeds (upper limit)
} lehmer_state_t;

static inline int32_t lehmer_seed_bind(int32_t val, uint32_t mod) {
    return (int32_t) ((val + mod) % mod);
}

void lehmer_generate_modulo(lehmer_state_t* state) {
    const uint32_t a = LEHMER_MULTIPLIER;
    const uint32_t m = LEHMER_MODULUS;

    uint32_t i = state->index;
    int32_t z = state->seed[i];

    int32_t r = (int32_t) (a * z) % m;
    state->seed[i] = lehmer_seed_bind(r, m);
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
    state->seed[i] = lehmer_seed_bind(y, m);
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
    state->seed[i] = lehmer_seed_bind(d, m);
}

void lehmer_generate_jump(lehmer_state_t* state) {
    const uint32_t a = LEHMER_JUMP;
    const uint32_t m = LEHMER_MODULUS;
    const uint32_t q = LEHMER_QUOTIENT;
    const uint32_t r = LEHMER_REMAINDER;

    uint32_t i = state->index;
    int32_t z = state->seed[i];

    int32_t j = (int32_t) ((a * (z % q)) - (r * (z / q)));
    state->seed[i] = lehmer_seed_bind(j, m);
}

// Generalized Lehmer sequence generator
void lehmer_generate(
    lehmer_state_t* state, void (*generate)(lehmer_state_t*), int32_t seed
) {
    // Initialize the first seed
    state->seed[0] = seed;

    // Generate sequence of seeds
    for (uint32_t i = 1; i < state->size; i++) {
        state->index = i - 1;
        generate(state); // Apply the generation function
    }
}

// Generates a sequence using the current time as seed (non-deterministic)
void lehmer_generate_time(
    lehmer_state_t* state, void (*generate)(lehmer_state_t*)
) {
    // Get the current time as seed
    time_t now = time(NULL);
    if (-1 == now) {
        lehmer_generate(state, generate, (int32_t) now);
    } else {
        lehmer_generate(state, generate, (int32_t) LEHMER_SEED);
    }
}

// Create and initialize the state with dynamic stream handling
lehmer_state_t* lehmer_state_create(uint32_t size, int32_t seed) {
    // Allocate memory for managing the LCG PRNG state
    lehmer_state_t* state = (lehmer_state_t*) malloc(sizeof(lehmer_state_t));
    if (NULL == state) {
        return NULL;
    }

    // Initialize the index to 0
    state->index = 0;
    // Default to a size of 256 if size is 0
    state->size = (size == 0) ? LEHMER_SIZE : size;

    // Allocate memory for generating seeds
    state->seed = (int32_t*) malloc(sizeof(int32_t) * state->size);
    if (NULL == state->seed) {
        free(state);
        return NULL;
    }

    // Initialize the seed state object
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

// select a seed from the current state
static inline uint32_t
lehmer_state_select(lehmer_state_t* state, uint32_t index) {
    // bind the index to the size of seed else nullify to 0.
    return (0 == state->size) ? 0 : index % state->size;
}

void lehmer_state_reseed(
    lehmer_state_t* state, void (*generate)(lehmer_state_t*), uint32_t index
) {
    // Prevent out-of-bounds indexing
    state->index = lehmer_state_select(state, index);
    // Get the selected seed
    int32_t seed = state->seed[state->index];
    // Generate a new stream based on this seed
    lehmer_generate(state, generate, seed);
}

int32_t lehmer_seed_get(lehmer_state_t* state) {
    // Prevent out-of-bounds indexing
    state->index = lehmer_state_select(state, state->index);
    return state->seed[state->index];
}

void lehmer_seed_set(
    lehmer_state_t* state, void (*generate)(lehmer_state_t*), int32_t value
) {
    // Ensure seed is within the modulus range
    int32_t seed = (int32_t) (value % LEHMER_MODULUS);
    // Generate a new stream based on this seed
    lehmer_generate(state, generate, seed);
}

// @note: generates a number in the range 0.0 to 1.0
float lehmer_seed_normalize(lehmer_state_t* state) {
    int32_t seed = state->seed[state->index];
    seed = lehmer_seed_bind(seed, LEHMER_MODULUS);
    return (float) seed / (float) LEHMER_MODULUS;
}

// Generalized random number generation
float lehmer_random(lehmer_state_t* state, void (*generate)(lehmer_state_t*)) {
    generate(state);
    return lehmer_seed_normalize(state);
}
