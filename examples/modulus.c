/**
 * Copyright © 2024 Austin Berrio
 *
 * @file examples/modulus.c
 *
 * @brief Minimal example showcasing the core algorithm.
 *
 * gcc -o modulus examples/modulus.c
 */

#include <stdio.h>

#define LIMIT 10

int main(void) {
    // signed int wraps around when overflows occur
    signed int position = 0; // this cannot overflow
    // unsigned int simply overflows and does not wrap around
    const unsigned int boundary = 256; // this can overflow

    // step backwards over each iteration
    for (unsigned int i = 0; i < LIMIT; i++) {
        // negate the position and bind it to the boundary
        position = (position - 1) % boundary;
        // output the current position
        printf("position: %d\n", position);
    }

    // return on success
    return 0;
}
