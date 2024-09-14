/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/prime.c
 *
 * @note Will need Mersenne twister for enhanced statistical properties. Use
 * lehmer for now since that's all that's currently implemented.
 *
 * @ref Miller-Rabin Primality Test
 * - https://rosettacode.org/wiki/Miller%E2%80%93Rabin_primality_test
 */

#include "prime.h"

#include <stdbool.h>
#include <stddef.h>

prime_sample_t* prime_sample_create(uint32_t size) {
    if (2 > size) {
        LOG_ERROR("Prime number sample size must be greater than 2.\n");
        return NULL;
    }

    // Allocate memory for sampling
    prime_sample_t* sample = (prime_sample_t*) malloc(sizeof(prime_sample_t));
    // Allocate memory for number of sampled data points
    sample->data           = malloc(size * sizeof(int));

    uint32_t j = 0;

    // Handle the first prime (2) directly.
    sample->data[j++] = 2;

    // Generate primes starting from 3
    for (uint32_t i = 3; i <= size; i += 2) {
        bool is_prime = true;

        for (uint32_t divisor = 3; divisor * divisor <= i; divisor += 2) {
            if (i % divisor == 0) {
                is_prime = false;
                break;
            }
        }

        if (is_prime) {
            sample->data[j++] = i;
        }
    }

    // Resize array to actual number of primes found
    sample->data = realloc(sample->data, j * sizeof(int));
    sample->size = j;

    return sample;
}

void prime_sample_free(prime_sample_t* sample) {
    if (sample == NULL) {
        LOG_ERROR("Sample data is NULL.\n");
        return;
    }

    if (sample->data) {
        free(sample->data);
    }

    free(sample);
}
