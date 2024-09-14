/**
 * Copyright © 2024 Austin Berrio
 *
 * @file test.h
 *
 * @brief A simple toolkit for writing test suites.
 * - Readability above all else.
 * - Pragmatism throughout.
 * - Kept simple at heart.
 */

#ifndef TEST_H
#define TEST_H

// Includes

#include "float_is_close.h" // *_is_close(); float_ or double_
#include "logger.h" // LOG_ERROR, LOG_INFO, LOG_*

#include <stdint.h>

// POSIX

// @ref http://man7.org/linux/man-pages/man2/clock_gettime.2.html

#define _POSIX_C_SOURCE   199309L
#define __USE_POSIX199309 _POSIX_C_SOURCE

// Macros

#define ASSERT_EQUAL(actual, expected, meta) \
    do { \
        if ((actual) != (expected)) { \
            LOG_ERROR("Test '%s' failed: %s", (meta)->name, (meta)->message); \
            (meta)->state = TEST_FAILED; \
        } else { \
            (meta)->state = TEST_PASSED; \
        } \
    } while (0)

#define ASSERT_EQUAL_STRING(actual, expected, meta) \
    do { \
        if (strcmp((actual), (expected)) != 0) { \
            LOG_ERROR("Test '%s' failed: %s", (meta)->name, (meta)->message); \
            (meta)->state = TEST_FAILED; \
        } else { \
            (meta)->state = TEST_PASSED; \
        } \
    } while (0)

#define ASSERT_EQUAL_FLOAT(actual, expected, significand, meta) \
    do { \
        if (float_is_close(actual, expected, significand)) { \
            LOG_ERROR("Test '%s' failed: %s", (meta)->name, (meta)->message); \
            (meta)->state = TEST_FAILED; \
        } else { \
            (meta)->state = TEST_PASSED; \
        } \
    } while (0)

// Aliases

// @note Order should not matter with enumerations - but I understand why there
// may be edge cases which depend upon the context.

// @note Consider automating this with a user defined function.
// This gets messy fast.
typedef enum TestDataType {
    TEST_TYPE_INTEGER = 0,
    TEST_TYPE_FLOAT   = 1,
    TEST_TYPE_STRING  = 2,
    // Add support for struct or complex types later
} test_data_t;

typedef enum TestState {
    TEST_PASSED  = 0x1,
    TEST_FAILED  = 0x2,
    TEST_SKIPPED = 0x4,
    TEST_RUNNING = 0x8,
    TEST_PENDING = 0x10
} test_state_t;

// @note Consider refactoring if this becomes bloated.
// Avoid "The Blob" at all costs, or become absorbed by your own peril.
typedef struct TestMetaData {
    test_state_t state;
    const char*  name;
    const char*  message;
    void*        expected;
    void*        actual;
    uint64_t     elapsed_time_ns; // Track elapsed time in nanoseconds
} test_meta_t; // e.g. data->state

typedef struct TestParameter {
    void**      cases; // Array of void pointers for each test case
    uint32_t    size; // Number of parameters
    test_data_t type; // Type descriptor
} test_parameter_t; // e.g. parameter->cases

typedef void (*test_callback_t)(test_meta_t* data);

typedef struct {
    const char*     name;
    test_callback_t function;
} test_case_t;

// Prototypes

/**
 * @brief Checks if the provided `test_parameter_t` matches the expected data
 * type, making it more concise and reducing redundancy throughout your
 * codebase.
 */
int test_data_type_check(test_parameter_t* param, test_data_t expected_type);

/**
 * @brief Runs a single test case within a custom suite by executing its
 * callback function with the supplied `test_meta_t`.
 */
void test_case_run(test_case_t* test);

/**
 * @brief A simple lightweight test runner that sequentially executes all
 * provided test cases in the given array, making it easy to run the entire
 * test suite from one place instead of manually calling each individual test
 * case.
 */
void test_suite_run(test_case_t* tests, uint32_t num_tests);

#endif // TEST_H
