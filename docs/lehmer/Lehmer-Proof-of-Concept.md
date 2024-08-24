# Lehmer RNG Exploration

## 1. Introduction
The Lehmer Random Number Generator (RNG) is a classic linear congruential generator used for producing pseudo-random sequences. This document summarizes the exploration and implementation of a Lehmer RNG in Python, including the testing of primality for key parameters.

## 2. Implementation Overview
A Python proof of concept was developed to mirror the Lehmer RNG implementation in C. The primary focus was on understanding the impact of various multipliers and modulus values on the generator's output.

- **Parameters:**
  - **Multiplier (`a`)**: The value by which the seed is multiplied.
  - **Seed (`z`)**: The starting point for the RNG sequence.
  - **Modulus (`m`)**: The value by which the product of `a` and `z` is divided to keep the output within a specific range.

## 3. Primality Testing
Primality testing is crucial for ensuring that the chosen parameters lead to a full-period sequence, which is desirable in RNGs.

- **Challenges:**
  - Primality testing for large numbers is computationally expensive.
  - The naive implementation of primality testing is slow, especially for large primes like `2^61-1`.

- **Results:**
  - `16807` is **not prime**.
  - `48271` is **prime**.
  - `2^31-1` and `2^61-1` are both **prime** (Mersenne primes).
  - Using pre-calculated prime values does not significantly affect the performance of the generator.

## 4. Output and Analysis
The Python implementation generated sequences based on the chosen parameters, and the results were analyzed for consistency and correctness.

- **Example Output:**
  ```plaintext
  115541394
  283598515
  1523151587
  652633738
  ...
  ```
- **Observations:**
  - The generator produced consistent outputs across multiple runs.
  - Non-prime multipliers like `16807` could lead to shorter periods and less reliable random sequences.

## 5. Future Considerations
Several aspects need further exploration and refinement:

- **Normalization and Floating-Point Range:**
  - Important for the C implementation to ensure that generated values are suitable for various applications.
  - Possible approaches include scaling the output to a desired range.

- **Potential Optimizations:**
  - Implementing the Miller-Rabin primality test for faster verification of large primes.
  - Considering asynchronous calculations to handle large primes more efficiently.

## 6. Conclusion
This exploration provided valuable insights into the functioning of the Lehmer RNG, particularly the importance of using prime values for key parameters. The next steps include refining the C implementation and further optimizing the Python proof of concept.

## 7. References
- [Mersenne Primes](https://www.mersenne.org/primes/)
- [Miller-Rabin Primality Test](https://www.geeksforgeeks.org/primality-test-set-3-miller-rabin/)
- [Park, Stephen K. "Random number generators: good ones are hard to find."](https://dl.acm.org/doi/10.1145/63039.63042)
