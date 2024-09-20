/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/validator.c
 *
 * @brief Minimal test cases
 */

#include "lehmer.h"
#include "logger.h"

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_SEEDS 10

/** Debugging */

// @brief Valid position for selected state at position 0 using LEHMER_SEED
// @ref python -m lehmer.cli -v -i 10 -r 0
// `python -m lehmer.cli -h` for more info
int32_t expected_sequence[LEHMER_SIZE] = {
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

#define LEHMER_ASSERT_INTEGER(iteration, expected, current) \
    if ((expected) != (current)) { \
        LOG_ERROR( \
            "Iteration %zu: Expected %d, Got %d\n", \
            iteration, \
            expected, \
            current \
        ); \
        exit(1); \
    }

#define LEHMER_ASSERT_FLOAT(iteration, expected, current) \
    if (!((fabsf(expected - current)) < (0.000001f))) { \
        LOG_ERROR( \
            "Iteration %zu: Expected %.7f, Got %.7f\n", \
            iteration, \
            expected, \
            current \
        ); \
        exit(1); \
    }

int test_lehmer_generator_modulo(lehmer_state_t* state, int32_t* expected) {
    for (uint32_t i = 0; i < MAX_SEEDS; i++) {
        lehmer_position_set(state, i);
        int32_t seed = lehmer_sequence_get(state);
        printf("Iteration %u: Expected %d, Got %d\n", i, expected[i], seed);
        LEHMER_ASSERT_INTEGER(i, expected[i], seed);
    }
}

/** CLI */

void print_usage(char* argv[]) {
    fprintf(stderr, "Usage:\n");
    fprintf(
        stderr,
        "\t%s [--help] "
        "[--verbose <0|1>] "
        "[--test <0|1>] "
        "[--position <unsigned integer>]\n",
        argv[0]
    );
}

// Define command line option structure
static struct option long_options[]
    = {{"help", optional_argument, 0, 'h'},
       {"verbose", required_argument, 0, 'v'},
       {"test", required_argument, 0, 't'},
       {"position", required_argument, 0, 's'},
       {0, 0, 0, 0}};

int main(int argc, char* argv[]) {
    uint32_t position = 0; // default position is 0
    bool test = false;
    bool verbose = false;

    while (true) {
        static int opt_index = 0;

        int c = getopt_long(argc, argv, "h:s:n:m:", long_options, &opt_index);

        if (c == -1) {
            break;
        }

        switch (c) {
            case 'h':
                print_usage(argv);
                return EXIT_SUCCESS;
            case 'v':
                // Output state every iteration
                verbose = (bool) atoi(optarg) ? true : false;
                break;
            case 't':
                // Enable testing initial position
                test = (bool) atoi(optarg) ? true : false;
                break;
            case 's':
                // Convert seed input
                position = (uint32_t) atoi(optarg);
                printf("Setting position to: %d\n", position);
                break;
            default:
                print_usage(argv);
                return EXIT_FAILURE;
        }
    }

    lehmer_state_t* state = lehmer_state_create(LEHMER_SEED, LEHMER_SIZE);

    if (test) {
        test_lehmer_generator_modulo(state, expected_sequence);
    } else {
        lehmer_position_set(state, position);

        for (uint32_t i = 0; i < MAX_SEEDS; i++) {
            lehmer_position_set(state, i);
            int32_t seed = lehmer_sequence_get(state);

            if (verbose) {
                lehmer_state_print(state);
            } else {
                printf("Iteration %u: Seed %d\n", i, seed);
            }
        }
    }

    int32_t seed = lehmer_sequence_get(state);
    float output = lehmer_seed_normalize_to_float(seed);
    printf("Normalized Seed: %.9f\n", output);

    lehmer_state_free(state);
    return 0;
}
