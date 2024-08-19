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
            = (uint64_t) (A256 * (state->seed[i - 1] % quotient) - remainder * (state->seed[i - 1] / quotient));
    }

    state->initialized = true;
}

// Generate the next random number
double lehmer_generate(lehmer_state_t* state) {
    const uint64_t quotient  = MODULUS / MULTIPLIER;
    const uint64_t remainder = MODULUS % MULTIPLIER;

    // Explicitly typecast signed results
    uint64_t new_seed = (uint64_t) (MULTIPLIER * (state->seed[state->stream] % quotient)
                                    - remainder * (state->seed[state->stream] / quotient));

    state->seed[state->stream] = new_seed;

    return ((double) state->seed[state->stream] / MODULUS);
}

// Determine a floating-point equality within a specified tolerance
bool float_is_close(float a, float b, int32_t significand) {
    // Numbers are equal
    if (a == b) {
        return true;
    }

    // Arguments are not numbers
    if (isinf(a) || isinf(b) || isnan(a) || isnan(b)) {
        return false;
    }

    // Calculate the minimum tolerance based on the significand
    significand         = (0.0f > significand) ? significand : -significand;
    float min_tolerance = powf(10.0f, significand);
    // Calculate the maximum tolerance based on the scale of the numbers
    float max_tolerance = min_tolerance * fmaxf(fabsf(a), fabsf(b));

    // Compare the absolute difference between the numbers within the tolerance range
    return fabsf(a - b) <= fmaxf(max_tolerance, min_tolerance);
}

// Determine a floating-point equality within a specified tolerance
bool double_is_close(double a, double b, int64_t significand) {
    // Numbers are equal
    if (a == b) {
        return true;
    }

    // Arguments are not numbers
    if (isinf(a) || isinf(b) || isnan(a) || isnan(b)) {
        return false;
    }

    // Calculate the minimum tolerance based on the significand
    significand          = (0.0 > significand) ? significand : -significand;
    double min_tolerance = pow(10.0, significand);
    // Calculate the maximum tolerance based on the scale of the numbers
    double max_tolerance = min_tolerance * fmax(fabs(a), fabs(b));

    // Compare the absolute difference between the numbers within the tolerance range
    return fabs(a - b) <= fmax(max_tolerance, min_tolerance);
}
