/**
 * Copyright © 2024 Austin Berrio
 *
 * @file src/prime.c
 *
 * @note Will need Mersenne twister for enhanced statistical properties. Use
 * Lehmer LCG PRNG for now since that's mostly implemented.
 *
 * @ref Miller-Rabin Primality Test
 * - https://rosettacode.org/wiki/Miller%E2%80%93Rabin_primality_test
 */

#include "prime.h"
#include "lehmer.h"

#include <stdbool.h>
#include <stddef.h>

int32_t
prime_modular_exponent(int32_t base, int32_t exponent, int32_t modulus) {
    int32_t result = 1;
    base           = base % modulus;

    while (0 < exponent) {
        if (1 == (exponent % 2)) {
            result = (result * base) % modulus;
        }
        exponent = exponent >> 1; // Divide exp by 2
        base     = (base * base) % modulus;
    }

    return result;
}

bool prime_miller_rabin(int32_t n, int16_t k) {
    if (n <= 1 || (n > 2 && n % 2 == 0)) {
        return false;
    }

    int32_t s = n - 1;
    while (0 == (s & 1)) {
        s >>= 1;
    }

    for (int16_t i = 0; i < k; ++i) {
        // Generate a random base in the range [2, n-2]
        int32_t a = 2 + rand() % (n - 3);

        // Compute a^s mod n
        int32_t x = prime_modular_exponent(a, s, n);

        if (x == 1 || x == n - 1) {
            continue;
        }

        // Repeat squaring up to k times
        bool is_composite = true;
        for (int32_t r = s; r != n - 1; r *= 2) {
            x = (x * x) % n;

            if (x == 1) {
                return false;
            }
            if (x == n - 1) {
                is_composite = false;
                break;
            }
        }

        if (is_composite) {
            return false;
        }
    }

    return true;
}

prime_sample_t* prime_sample_create(uint32_t size) {
    if (2 > size) {
        LOG_ERROR("Prime number sample size must be greater than 2.\n");
        return NULL;
    }

    // Allocate memory for sampling
    prime_sample_t* sample = (prime_sample_t*) malloc(sizeof(prime_sample_t));
    // Allocate memory for number of sampled data points
    sample->data           = malloc(size * sizeof(int16_t));

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
    sample->data = realloc(sample->data, j * sizeof(int16_t));
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
