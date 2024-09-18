/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/simple_impl.c
 *
 * @brief Minimal implementation utilizing the core library.
 *
 * @note See lehmer.h and lehmer.c for details.
 * @ref lehmer.h is well documented and contains more high-level details.
 * @ref lehmer.c is the core implementation for the primary interface.
 */

#include "lehmer.h"
#include "logger.h"

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#define MAX_SEEDS 10

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
    // uint32_t stream = 0; // default stream is 0

    lehmer_state_t* state = lehmer_state_create(LEHMER_SIZE, LEHMER_SEED);

    lehmer_state_print(state);

    float output = lehmer_seed_normalize_to_float(state);
    printf("Normalized Seed: %.9f\n", output);

    lehmer_state_free(state);
    return 0;
}
