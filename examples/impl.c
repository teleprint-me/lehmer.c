/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/lehmer.c
 *
 * @brief Minimal implementation
 */

#include "lehmer.h"
#include "logger.h"

#include <stdio.h>

#define MAX_SEEDS 10

// @brief Valid stream for selected state at stream 0 using LEHMER_SEED
// @ref python -m lehmer.cli -v -i 10 -r 0
// `python -m lehmer.cli -h` for more info
int32_t expected_stream_0[MAX_SEEDS] = {
    115541394,
    283598515,
    1523151587,
    652633738,
    1845549155,
    291648857,
    1426670162,
    1289797906,
    2136310349,
    1819611286,
};

#define LEHMER_ASSERT(iteration, expected, current) \
    if (expected != current) { \
        LOG_ERROR( \
            "Iteration %u: Expected %d, Got %d\n", \
            iteration, \
            expected, \
            current \
        ); \
        exit(1); \
    }

int test_lehmer_generator_modulo(
    lehmer_state_t* state, int32_t* values, uint32_t stream
) {
    for (uint32_t i = 0; i < MAX_SEEDS; i++) {
        lehmer_state_select(state, stream);
        lehmer_generate_modulo(state);
        int32_t current  = lehmer_seed_get(state);
        int32_t expected = values[i];
        printf("Iteration %u: Expected %d, Got %d\n", i, expected, current);
        LEHMER_ASSERT(i, expected, current);
    }
}

int main(void) {
    lehmer_state_t* state = lehmer_state_create(LEHMER_STREAMS, LEHMER_SEED);
    test_lehmer_generator_modulo(state, expected_stream_0, 0);

    float output = lehmer_seed_normalize(state);
    printf("Normalized Seed: %.9f\n", output);

    lehmer_state_free(state);
    return 0;
}
