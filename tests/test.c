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

// Includes

#include "float_is_close.h" // *_is_close(); float_ or double_
#include "logger.h" // LOG_ERROR, LOG_INFO, LOG_*

#include <stdint.h>

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

// @note A helper function for type checking might make this even more concise,
// so we don’t repeat checks throughout.
int test_data_type_check(test_parameter_t* param, test_data_t expected_type);
