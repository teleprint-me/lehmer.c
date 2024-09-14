/**
 * Copyright © 2024 Austin Berrio
 *
 * @file test.c
 *
 * @brief A simple toolkit for writing test suites.
 * - Readability above all else.
 * - Pragmatism throughout.
 * - Kept simple at heart.
 */

#include "test.h"

#include <time.h>

void test_case_run(test_case_t* test) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    test_meta_t meta
        = {.state = TEST_RUNNING, .name = test->name, .elapsed_time_ns = 0};
    test->function(&meta);

    clock_gettime(CLOCK_MONOTONIC, &end);
    meta.elapsed_time_ns
        = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    if (meta.state == TEST_PASSED) {
        LOG_INFO(
            "Test '%s' PASSED in %lu ns", test->name, meta.elapsed_time_ns
        );
    } else {
        LOG_ERROR(
            "Test '%s' FAILED in %lu ns", test->name, meta.elapsed_time_ns
        );
    }
}
