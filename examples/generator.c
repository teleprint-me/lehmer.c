/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/generator.c
 *
 * @brief Minimal implementation utilizing the core library.
 *
 * @note See lehmer.h and lehmer.c for details.
 * @ref lehmer.h is well documented and contains more high-level details.
 * @ref lehmer.c is the core implementation for the primary interface.
 */

#include "lehmer.h"

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_SEEDS 10

int main(void) {
    lehmer_state_t* state = lehmer_state_create(LEHMER_SEED, LEHMER_SIZE);

    lehmer_state_print(state);

    int32_t current_seed = lehmer_get_current_seed(state);
    printf("current seed = %d\n", current_seed);

    float normalized_seed = lehmer_seed_normalize_to_float(current_seed);
    printf("normalized seed: %.9f\n", normalized_seed);

    float output_random = lehmer_random_modulo(state);
    printf("random modulo: %.9f\n", output_random);

    lehmer_state_print(state);

    lehmer_state_free(state);
    return 0;
}
