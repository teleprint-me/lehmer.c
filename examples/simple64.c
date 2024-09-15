/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/simple64.c
 *
 * @brief Proof of concept for the Lehmer RNG in pure C from scratch
 *
 * @note The only difference between the 64-bit and 32-bit implementations are
 * the data types.
 *
 * - The 64-bit utilizes double, uint64_t, and int64_t
 * - The 32-bit utilizes float, uint32_t, and int32_t
 * - The format specifiers for printing output are adjusted from llu to lu
 *
 * No other changes are made and both source files should mirror one another.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @param LEHMER_MODULUS Mersenne prime number used as modulus (2^31 - 1)
 *
 * @note Must be a mersenne prime number
 * - 32-bit: 2^31 - 1 = 2147483647
 * - 64-bit: 2^61 - 1 = 2305843009213693951
 */
#define LEHMER_MODULUS    2147483647

/**
 * @param LEHMER_MULTIPLIER Chosen for safe 32-bit arithmetic (16807)
 *
 * @note Must be a prime number
 */
#define LEHMER_MULTIPLIER 48271

/**
 * @param lehmer_seed Global variable used to track the current seed
 */
uint64_t lehmer_seed = 0;

void lehmer_set_seed(uint64_t value) {
    lehmer_seed = value;
}

uint64_t lehmer_get_seed(void) {
    return lehmer_seed;
}

// @note: the generate functions generate the next seed

uint64_t lehmer_generate_gamma(void) {
    uint64_t q = LEHMER_MODULUS / LEHMER_MULTIPLIER;
    uint64_t r = LEHMER_MODULUS % LEHMER_MULTIPLIER;
    int64_t  y = (int64_t) LEHMER_MULTIPLIER * (lehmer_seed % q)
                - r * (lehmer_seed / q);
    printf("q: %llu, r: %llu, y: %lld\n", q, r, y); // Debug print
    return (y > 0) ? (uint64_t) y : (uint64_t) (y + LEHMER_MODULUS);
}

uint64_t lehmer_generate_delta(void) {
    uint64_t q = LEHMER_MODULUS / LEHMER_MODULUS;
    int64_t  d
        = (int64_t) ((lehmer_seed / q)
                     - (LEHMER_MULTIPLIER * lehmer_seed / LEHMER_MODULUS));
    printf("q: %llu, d: %lld\n", q, d); // Debug print
    return (d > 0) ? (uint64_t) d : (uint64_t) (d + LEHMER_MODULUS);
}

uint64_t lehmer_generate_modulo(void) {
    return (uint64_t) (LEHMER_MULTIPLIER * lehmer_seed) % LEHMER_MODULUS;
}

// @note: generates a number in the range 0.0 to 1.0
double lehmer_normalize(void) {
    return (double) lehmer_seed / (double) LEHMER_MODULUS;
}

// modulo generation
double lehmer_random_modulo(void) {
    lehmer_seed = lehmer_generate_modulo();
    return lehmer_normalize();
}

// gamma generation
double lehmer_random_gamma(void) {
    lehmer_seed = lehmer_generate_gamma();
    return lehmer_normalize();
}

// delta generation
double lehmer_random_delta(void) {
    lehmer_seed = lehmer_generate_delta();
    return lehmer_normalize();
}

int main(void) {
    lehmer_set_seed(123456789);
    printf("lehmer_seed: %llu\n", lehmer_get_seed());

    // generate lehmer gamma values
    for (size_t i = 0; i < 10; i++) {
        printf("%zu: %.9f\n", i, lehmer_random_gamma());
    }

    return 0;
}
