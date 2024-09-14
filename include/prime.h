/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/prime.h
 */

#ifndef PRIME_H
#define PRIME_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Data structure representing the sampled prime numbers
 *
 * @param data A pointer to the sampled data
 * @param size The number of data points
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
 * @param a Base integer
 * @param b Exponent
 * @param m Modulus
 * @return Computed result of a^b mod m
 */
int32_t prime_modular_exponent(uint32_t a, uint32_t b, uint32_t m);

/**
 * @brief Determine whether a given number is likely prime or not
 *
 * This function uses the Miller-Rabin primality test to check if an odd
 * integer `n` with at least 5 digits is probably prime. The hyperparameter
 * `k` determines the accuracy of the test (higher values increase
 * confidence but decrease performance).
 *
 * @param n Odd integer to be tested for primality
 * @param k Hyperparameter that controls the number of iterations in
 * Miller-Rabin's algorithm
 * @return true if prime, else false
 *
 * @note Configuring k is a trade off between performance and accuracy,
 * depending on the size of n. A typical value of k ranges from 5 to 50 for
 * most applications.
 */
bool prime_miller_rabin(uint32_t n, int16_t k);

/**
 * @brief Create a structure containing an array of prime numbers as sample
 * data.
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
 * @param samples Pointer to the sample_data_t structure.
 */
void prime_sample_free(prime_sample_t* samples);

#endif // PRIME_H
