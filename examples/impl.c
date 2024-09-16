/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/lehmer.c
 *
 * @brief Minimal implementation
 */

#include "lehmer.h"
#include "logger.h"

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

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

int test_lehmer_generator_modulo(lehmer_state_t* state, int32_t* expected) {
    for (uint32_t i = 0; i < MAX_SEEDS; i++) {
        lehmer_generate_modulo(state);
        int32_t current = lehmer_seed_get(state);
        printf("Iteration %u: Expected %d, Got %d\n", i, expected[i], current);
        LEHMER_ASSERT(i, expected[i], current);
    }
}

void print_usage(char* argv[]) {
    fprintf(stderr, "Usage:\n");
    fprintf(
        stderr,
        "\t%s [--help] "
        "[--verbose <0|1>] "
        "[--test <0|1>] "
        "[--stream <unsigned integer>]\n",
        argv[0]
    );
}

// Define command line option structure
static struct option long_options[]
    = {{"help", optional_argument, 0, 'h'},
       {"verbose", required_argument, 0, 'v'},
       {"test", required_argument, 0, 't'},
       {"stream", required_argument, 0, 's'},
       {0, 0, 0, 0}};

int main(int argc, char* argv[]) {
    uint32_t stream = 0; // default stream is 0
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
                // Enable testing initial stream
                test = (bool) atoi(optarg) ? true : false;
                break;
            case 's':
                // Convert seed input
                stream = (uint32_t) atoi(optarg);
                printf("Setting stream to: %d\n", stream);
                break;
            default:
                print_usage(argv);
                return EXIT_FAILURE;
        }
    }

    lehmer_state_t* state = lehmer_state_create(LEHMER_STREAMS, LEHMER_SEED);
    lehmer_state_select(state, stream);
    if (test) {
        test_lehmer_generator_modulo(state, expected_stream_0);
    } else {
        for (uint32_t i = 0; i < MAX_SEEDS; i++) {
            lehmer_generate_modulo(state);
            if (verbose) {
                lehmer_state_print(state);
            } else {
                int32_t current = lehmer_seed_get(state);
                printf("Iteration %u: Seed %d\n", i, current);
            }
        }
    }

    float output = lehmer_seed_normalize(state);
    printf("Normalized Seed: %.9f\n", output);

    lehmer_state_free(state);
    return 0;
}
