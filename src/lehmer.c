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
lehmer_state_t* lehmer_create_state(size_t size) {
    lehmer_state_t* state = (lehmer_state_t*) malloc(sizeof(lehmer_state_t));
    state->seed           = (uint64_t*) malloc(sizeof(uint64_t) * size);
    state->stream         = 0;
    state->size           = size;
    state->initialized    = false;

    // Initialize all available streams and ensure seed is within MODULUS range
    for (size_t i = 0; i < size; i++) {
        state->seed[i] = (((uint64_t) time(NULL)) + i * A256) % MODULUS;
    }

    return state;
}

// Free the allocated memory
void lehmer_free_state(lehmer_state_t* state) {
    if (state) {
        if (state->seed) {
            free(state->seed);
        }
        free(state);
    }
}

void lehmer_set_seed(lehmer_state_t* state, uint64_t value) {
    // Ensure seed is within the modulus range
    value                      = value % MODULUS;
    state->seed[state->stream] = value;
}

uint64_t lehmer_get_seed(lehmer_state_t* state) {
    return state->seed[state->stream];
}

// Stream selection should not trigger seeding; assume streams are initialized.
void lehmer_select_stream(lehmer_state_t* state, size_t stream) {
    state->stream = stream % state->size;
}

// Initialize the RNG state with seeds; decoupled from stream selection.
void lehmer_seed_streams(lehmer_state_t* state, uint64_t value) {
    const uint64_t quotient      = MODULUS / A256;
    const uint64_t remainder     = MODULUS % A256;
    const size_t   stream_backup = state->stream;

    // Select and set the initial stream
    lehmer_select_stream(state, 0);
    lehmer_set_seed(state, value);

    state->stream = stream_backup;

    // Initialize remaining streams based on the first one
    for (size_t i = 1; i < state->size; i++) {
        state->seed[i]
            = (uint64_t) (A256 * (state->seed[i - 1] % quotient)
                          - remainder * (state->seed[i - 1] / quotient));
    }

    state->initialized = true;
}

// Generate the next random number
double lehmer_generate(lehmer_state_t* state) {
    const uint64_t quotient  = MODULUS / MULTIPLIER;
    const uint64_t remainder = MODULUS % MULTIPLIER;

    // Explicitly typecast signed results
    uint64_t new_seed
        = (uint64_t) (MULTIPLIER * (state->seed[state->stream] % quotient)
                      - remainder * (state->seed[state->stream] / quotient));

    state->seed[state->stream] = new_seed;

    return ((double) state->seed[state->stream] / MODULUS);
}
