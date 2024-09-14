/**
 * Copyright © 2024 Austin Berrio
 *
 * @file test.c
 *
 * @brief A simple toolkit for writing test suites
 */

// Includes

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

// Aliases

typedef enum TestState {
    TEST_PASSED  = 0x1,
    TEST_FAILED  = 0x2,
    TEST_SKIPPED = 0x4,
    TEST_RUNNING = 0x8,
    TEST_PENDING = 0x10
} test_state_t;

typedef struct TestMetaData {
    test_state_t state;
    const char*  name;
    const char*  message;
    void*        expected;
    void*        actual;
} test_meta_t;

typedef struct TestParameter {
    void**   cases; // Array of void pointers for each test case
    uint32_t size;  // Number of parameters
} test_parameter_t; // e.g. test_parameter_t parameter->cases

typedef void (*test_callback_t)(test_meta_t* meta);

typedef struct {
    const char*     name;
    test_callback_t function;
} test_case_t;
