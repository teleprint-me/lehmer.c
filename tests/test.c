/**
 * @file test.c
 *
 * @brief A simple toolkit for writing test suites
 */

#include "logger.h" // LOG_ERROR, LOG_INFO, LOG_*

#include <stdint.h>

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
