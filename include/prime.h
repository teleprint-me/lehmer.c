/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/prime.h
 */

#ifndef PRIME_H
#define PRIME_H

#include <stdint.h>

/**
 * @brief Data structure representing the sampled prime numbers
 *
 * @param data The sampled data
 * @param size The number of data points
 */
typedef struct PrimeSample {
    int*     data; // The sampled data
    uint32_t size; // The number of data points
} prime_sample_t;

/**
 * @brief Create an array of prime numbers as sample data.
 *
 * @param size Number of prime numbers to generate.
 * @return Pointer to a dynamically allocated sample_data_t structure.
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
