/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/algorithm.c
 *
 * @brief Minimal example showcasing the core algorithm.
 *
 * gcc -o algorithm examples/algorithm.c
 */

#include <stdint.h>
#include <stdio.h>

#define LEHMER_MODULUS 2147483647 // Mersenne prime number (2^31 - 1)
#define LEHMER_MULTIPLIER 48271 // Prime number
#define LEHMER_SEED 123456789 // Default seed
#define LEHMER_SIZE 10 // Number of values to generate

// @brief Normalizes a seed to a float in the range 0.0 to 1.0
float lehmer_seed_normalize_to_float(int32_t seed) {
    return (float) seed / (float) LEHMER_MODULUS;
}

// @brief Normalizes a seed to a integer in the range 0 to m - 1
int32_t lehmer_seed_normalize_to_int(int32_t value, uint32_t modulus) {
    return (int32_t) (value % modulus);
}

// Generate the next random number in the sequence
int32_t lehmer_generate_modulo(int32_t z) {
    const uint32_t a = LEHMER_MULTIPLIER;
    const uint32_t m = LEHMER_MODULUS;

    // Lehmer formula: (a * z) % m
    int64_t temp = (int64_t) a * z; // Use int64_t to avoid overflow
    int32_t r = (int32_t) (temp % m); // Modulus operation
    return lehmer_seed_normalize_to_int(r, m);
}

int main(void) {
    int32_t seed = LEHMER_SEED; // Initial seed

    // Generate and print Lehmer RNG sequence
    for (uint32_t i = 0; i < LEHMER_SIZE; i++) {
        seed = lehmer_generate_modulo(seed);
        printf("Iteration %u: Seed = %d\n", i + 1, seed);
    }

    float random = lehmer_seed_normalize_to_float(seed);
    printf("Random Number = %.7f\n", random);

    return 0;
}
