# Lehmer RNG Exploration

## 1. Introduction

The Lehmer Random Number Generator (RNG) is a classic linear congruential
generator that has been widely studied and used for producing pseudo-random
sequences. This document outlines the exploration and implementation of a
Lehmer RNG in Python, serving as a foundational proof of concept for a
forthcoming C implementation.

## 2. Implementation Overview

A Python proof of concept was developed to closely mirror the planned Lehmer
RNG implementation in C. This prototype focuses on the critical aspects of the
generator, such as the choice of multipliers and modulus values, to evaluate
their impact on the generator's output.

- **Parameters:**

  - **Multiplier (`a`)**: A crucial value multiplied by the seed in each
    iteration to produce a new sequence value.
  - **Seed (`z`)**: The initial value from which the sequence generation
    begins.
  - **Modulus (`m`)**: The divisor used to ensure that the generator's output
    remains within a defined range.

- **Methods:**
  - **`random()`**: Generates the base random sequence using the multiplier and
    modulus.
  - **`y_random()`**: Implements a variant based on the gamma case.
  - **`d_random()`**: Implements a variant based on the delta case.

## 3. Primality Testing

Primality testing of the multiplier and modulus is critical for ensuring the
generator's full-period behavior, which is essential for producing high-quality
random sequences.

- **Challenges:**

  - **Computational Expense**: Testing large numbers for primality, such as
    `2^61-1`, is computationally intensive.
  - **Performance Considerations**: While using non-prime values can simplify
    the implementation, it can also reduce the generator's effectiveness by
    shortening the period.

- **Findings:**
  - **Multiplier Primality**:
    - `16807` is **not prime**.
    - `48271` is **prime**.
  - **Modulus Primality**:
    - `2^31-1` and `2^61-1` are both **Mersenne primes**.
  - **Impact on Performance**:
    - The choice of prime values for multipliers and modulus ensures a more
      reliable and full-period RNG.

## 4. Output and Analysis

The Python implementation of the Lehmer RNG generated consistent sequences
based on selected parameters. These sequences were analyzed to verify the
generator's correctness and repeatability.

- **Example Output**:
  ```plaintext
  115541394
  283598515
  1523151587
  652633738
  ...
  ```
- **Consistency**:
  - The generator produced predictable and consistent outputs across multiple
    runs, validating the correctness of the implementation.
  - **Non-Prime Multiplier Effects**: The use of a non-prime multiplier like
    `16807` could lead to shorter periods, potentially compromising the
    randomness of the sequence.

## 5. Normalization and Floating-Point Range

For practical applications, the RNG output needs to be normalized to a
floating-point range (e.g., 0 to 1). This step is particularly important for
the C implementation to ensure compatibility with various use cases.

- **Approach**:
  - Scale the output to fit within a desired floating-point range.
  - Investigate different normalization techniques to maintain accuracy and
    performance.

## 6. Potential Optimizations

Further refinement and optimization of the implementation can enhance both
performance and accuracy:

- **Primality Testing**:

  - Implement more efficient algorithms like the Miller-Rabin test to handle
    large prime numbers swiftly.
  - Precompute prime values for commonly used parameters to save computational
    resources during runtime.

- **Asynchronous Calculations**:
  - Explore asynchronous methods for handling the computational load associated
    with large primes, which could help in high-performance applications.

## 7. Conclusion

This exploration provided a solid understanding of the Lehmer RNG, emphasizing
the importance of using prime values for critical parameters. The next steps
involve refining the Python proof of concept and translating these findings
into a robust and optimized C implementation.

## 8. References

- [Prime Numbers](https://numbergenerator.org/numberlist/prime-numbers/10000-100000)
- [Mersenne Primes](https://www.mersenne.org/primes/)
- [Miller-Rabin Primality Test](https://www.geeksforgeeks.org/primality-test-set-3-miller-rabin/)
- [Park, Stephen K. "Random number generators: good ones are hard to find."](https://dl.acm.org/doi/10.1145/63039.63042)
