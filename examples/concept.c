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
 * @param LEHMER_MODULUS Mersenne prime number used as modulus (2^31 - 1)
 */
#define LEHMER_MODULUS     2147483647

/**
 * @param LEHMER_MULTIPLIER Multiplier is a prime number used to scale the seed
 */
#define LEHMER_MULTIPLIER  48271

/**
 * @param LEHMER_JUMP A prime number used as a multiplier to "jump" scale seeds
 */
#define LEHMER_JUMP        22937

/**
 * @param LEHMER_STREAM_SIZE Default number of streams
 */
#define LEHMER_STREAM_SIZE 256

// @note the size of seed and stream must be the same
// the general idea is that we select a seed
// then generate a new sequence from the selected seed

// plant the seeds that take root
typedef struct LehmerState {
    int32_t* seed; // Array of seeds representing the initial state
    uint32_t size; // The number of seeds (upper limit)
} lehmer_state_t;

int32_t lehmer_generate_modulo(lehmer_state_t* state, uint32_t index) {
    uint32_t a = LEHMER_MULTIPLIER;
    uint32_t m = LEHMER_MODULUS;
    int32_t z = state->seed[index]; // Initial seed?
    return (int32_t) ((a * z) % m);
}

void lehmer_generator(lehmer_state_t* state, int32_t seed) {
    // Initialize first seed
    state->seed[0] = seed;

    // Generate sequence of seeds
    for (uint32_t i = 1; i < state->size; i++) {
        int32_t z = state->seed[i - 1];
        state->seed[i] = (int32_t) ((LEHMER_MULTIPLIER * z) % LEHMER_MODULUS);
        if (state->seed[i] < 0) {
            state->seed[i] += LEHMER_MODULUS;
        }
    }
}

// @warning This will generate non-deterministic results
void lehmer_generator_time(lehmer_state_t* state) {
    time_t now = time(NULL); // Get the current time as the seed
    lehmer_generator(state, (int32_t) now); // Generate sequence based on time
}

// Create and initialize the state with dynamic stream handling
lehmer_state_t* lehmer_state_create(uint32_t size, int32_t seed) {
    lehmer_state_t* state = (lehmer_state_t*) malloc(sizeof(lehmer_state_t));
    if (NULL == state) {
        return NULL;
    }

    state->seed = (int32_t*) malloc(sizeof(int32_t) * size);
    if (NULL == state->seed) {
        free(state);
        return NULL;
    }

    state->size = (size == 0) ? 1 : size; // Ensure size is at least 1

    lehmer_generator(state, seed); // Generate initial seed sequence

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

// @note I don't know - can be index or position... index is less chars
uint32_t lehmer_bind_index(lehmer_state_t* state, uint32_t position) {
    return position % state->size; // Prevent out-of-bounds indexing
}

void lehmer_state_select(lehmer_state_t* state, uint32_t position) {
    uint32_t index = lehmer_bind_index(state, position);
    int32_t seed = state->seed[index]; // Get the selected seed
    lehmer_generator(state, seed); // Generate a new stream based on this seed
}

int32_t lehmer_seed_get(lehmer_state_t* state, uint32_t position) {
    uint32_t index = lehmer_bind_index(state, position);
    return state->seed[index];
}

void lehmer_seed_set(lehmer_state_t* state, uint32_t position, int32_t value) {
    uint32_t index = lehmer_bind_index(state, position);
    state->seed[index] = lehmer_generate_modulo(state, index);
}
