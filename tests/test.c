/**
 * @file test.c
 *
 * @brief A simple toolkit for writing test suites
 */

#include "logger.h" // LOG_ERROR, LOG_INFO, LOG_*

#include <stdint.h>

typedef enum {
    TEST_PASSED  = 0x1,
    TEST_FAILED  = 0x2,
    TEST_SKIPPED = 0x4,
    TEST_RUNNING = 0x8,
    TEST_PENDING = 0x10
} test_state_t;

typedef struct {
    uint32_t             state; // enum flags regarding test state
    const unsigned char* name;
    const unsigned char* message;
    void*                expected;
    void*                actual;
} test_meta_t;

typedef struct {
    void*    parameters; // list of values to test
    uint32_t size;       // number of parameters
} test_params_t;
