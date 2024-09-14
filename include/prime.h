/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/prime.h
 *
 * @note Will need Mersenne twister for enhanced statistical properties. Use
 * Lehmer LCG PRNG for now since that's mostly implemented.
 */

#ifndef PRIME_H
#define PRIME_H

#include "lehmer.h"

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Data structure representing the sampled prime numbers
 *
 * @param data A pointer to the sampled data
 * @param size The number of data points (must be greater than 0)
 */
typedef struct PrimeSample {
    int32_t* data; // The sampled data
    uint32_t size; // The number of data points
} prime_sample_t;

/**
 * @brief Computes a^b mod m, where a, b, and m are integers
 *
 * This function computes the result of raising `a` to the power of `b`,
 * then finds this value modulo `m`. The input values for `a`, `b`,
 * and `m` should be non-negative integers.
 *
 * @param a Base integer (signed int)
 * @param b Exponent (signed int)
 * @param m Modulus (unsigned int)
 * @return Computed result of a^b mod m
 *
 * @note This function assumes m > 0 and b >= 0. For a^0, the result is 1
 * (with any base a). Negative exponents are not supported.
 */
int32_t prime_modular_exponent(int32_t a, int32_t b, uint32_t m);

/**
 * @brief Determine whether a given number is likely prime or not
 *
 * This function uses the Miller-Rabin primality test to check if an odd
 * integer `n` with at least 5 digits is probably prime. The hyperparameter
 * `k` determines the accuracy of the test (higher values increase
 * confidence but decrease performance).
 *
 * @param state Structure representing the state of the LCG RNG
 * @param n Odd integer to be tested for primality (must be > 2)
 * @param k Hyperparameter that controls the number of iterations in
 * the Miller-Rabin algorithm. Higher `k` values improve accuracy but
 * decrease performance.
 * @return true if n is likely prime, otherwise false.
 *
 * @note Typical values of `k` range from 5 to 50. A value of `k = 5`
 * offers good performance and accuracy for smaller integers, while larger
 * values of `k` (e.g., 20-50) are useful for very large integers or
 * when high accuracy is needed.
 *
 * @ref Miller-Rabin Primality Test
 * - https://rosettacode.org/wiki/Miller%E2%80%93Rabin_primality_test
 */
bool prime_miller_rabin(lehmer_state_t* state, uint32_t n, uint16_t k);

/**
 * @brief Create an array of prime numbers as sample data.
 *
 * @param size Number of prime numbers to generate.
 * @return Pointer to a dynamically allocated prime_sample_t structure.
 *
 * @note It's cute <3
 */
prime_sample_t* prime_sample_create(uint32_t size);

/**
 * @brief Free the allocated sample data.
 *
 * @param samples Pointer to the prime_sample_t structure.
 */
void prime_sample_free(prime_sample_t* samples);

#endif // PRIME_H
