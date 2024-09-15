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

#include <getopt.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * @param LEHMER_MODULUS Mersenne prime number used as modulus (2^31 - 1)
 *
 * @note Must be a mersenne prime number
 * - 32-bit: 2^31 - 1 = 2147483647
 * - 64-bit: 2^61 - 1 = 2305843009213693951
 */
#define LEHMER_MODULUS    2147483647

/**
 * @param LEHMER_MULTIPLIER Multiplier used to scale the seed
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
    uint64_t z = (uint64_t) (LEHMER_MULTIPLIER * lehmer_seed) % LEHMER_MODULUS;
    printf("z: %llu\n", z); // Debug print
    return z;
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
    lehmer_seed = lehmer_generate_gamma();
    lehmer_seed = lehmer_generate_delta();
    return lehmer_normalize();
}

void print_usage(char* argv[]) {
    fprintf(stderr, "Usage:\n");
    fprintf(
        stderr,
        "\t%s [--seed <value>] [--count <number of random numbers to "
        "generate>] [--mode <g|m|d>]\n",
        argv[0]
    );
}

// Define command line option structure
static struct option long_options[]
    = {{"help", optional_argument, 0, 'h'},
       {"seed", required_argument, 0, 's'},
       {"count", required_argument, 0, 'n'},
       {"mode", required_argument, 0, 'm'}, // Adding mode option
       {0, 0, 0, 0}};

int main(int argc, char* argv[]) {
    int    seed  = 123456789;
    size_t count = 10;
    char   mode  = 'm'; // Default mode is modulo

    while (true) {
        static int opt_index = 0;

        int c = getopt_long(argc, argv, "h:s:n:m:", long_options, &opt_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 's':
                seed = atoi(optarg); // Convert seed input
                printf("Setting seed to: %d\n", seed);
                break;

            case 'n':
                count = atoi(optarg); // Convert count input
                break;

            case 'm':
                mode = optarg[0]; // Capture the first character of mode
                if (mode != 'g' && mode != 'm' && mode != 'd') {
                    fprintf(
                        stderr,
                        "Invalid mode. Use g (gamma), m (modulo), or d "
                        "(delta).\n"
                    );
                    print_usage(argv);
                    return EXIT_FAILURE;
                }
                break;

            case 'h':
                print_usage(argv);
                return EXIT_SUCCESS;

            default:
                print_usage(argv);
                return EXIT_FAILURE;
        }
    }

    lehmer_set_seed(seed); // Set seed for the generator

    // generate random numbers based on the selected mode
    for (size_t i = 0; i < count; i++) {
        double random_value;
        switch (mode) {
            case 'g':
                random_value = lehmer_random_gamma();
                break;

            case 'm':
                random_value = lehmer_random_modulo();
                break;

            case 'd':
                random_value = lehmer_random_delta();
                break;
        }

        printf("%zu: %.9f\n", i, random_value);
    }

    return EXIT_SUCCESS;
}
