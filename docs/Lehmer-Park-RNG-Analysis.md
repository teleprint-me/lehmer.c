# Lehmer-Park RNG Implementation and Analysis

## Overview

This document provides a detailed analysis and explanation of the key aspects
related to the Lehmer random number generator (RNG), with specific focus on the
constants and mathematical foundations as discussed in the paper, _Random
number generators: good ones are hard to find_ by Stephen Park.

## Key Concepts

### Lehmer's Algorithm

The Lehmer Random Number Generator is a simple and elegant method based on the equation $f(z) = az \mod m$.

- **Equation**: The core of Lehmer’s RNG is defined by the iterative equation
  $z_{n+1} = f(z_n)$, where $f(z) = a \cdot z \mod m$.
- **Parameters**:
  - **Modulus (m)**: A large prime integer.
  - **Multiplier (a)**: An integer in the range $2, \dots, m-1$.
- **Normalization**: The sequence is normalized to the unit interval via
  division by the modulus to produce the sequence $u_n = \frac{z_n}{m}$.

### Historical Context and Choices

- **Multiplier (a = 16807)**: Chosen due to hardware constraints at the time of
  Park's writing, specifically for systems with a 32-bit integer limit.
- **Modulus (m = 2147483647)**: A Mersenne prime, $2^{31} - 1$, chosen for its
  properties that suit the full period requirement and ease of computation with
  32-bit systems.

### Full Period Generating Function

- **Definition**: A function $f(z) = a \cdot z \mod m$ is a full period
  generator if it generates a sequence with a period of $p = m - 1$.
- **Primitive Root**: The multiplier $a$ must be a primitive root modulo $m$,
  meaning $a^n \mod m \neq 1$ for $n = 1, 2, \dots, m-2$.

### Mathematical Derivations

- **Fermat’s Theorem**: The full period of the sequence is guaranteed by
  Fermat's theorem, which states that $a^{m-1} \mod m = 1$ if $m$ is prime.
- **Factorization Attempt**: Park explored the idea of approximately factoring
  the modulus $m$ into $m = a \cdot q + r$, where $q = \frac{m}{a}$ and
  $r = m \mod a$.

### Correctness Over Randomness

- **Testing Method**: Park emphasized testing the correctness of RNG
  implementations over randomness. A simple test he proposed involves checking
  if the sequence starting from $z_1 = 1$ correctly produces
  $z_{10001} = 1043618065$.

### Implementation Considerations

- **Bounded Results**: The equation $f(z) = \gamma(z) + m \cdot \delta(z)$ is
  used to ensure all intermediate results are bounded by $m - 1$. Here:
  - $\gamma(z) = a \cdot (z \mod q) - r \cdot (z \div q)$
  - $\delta(z) = (z \div q) - (a \cdot z \div m)$
- **Significance of Constants**:
  - $\delta(z)$ is either 0 or 1.
  - Both $a \cdot (z \mod q)$ and $r \cdot (z \div q)$ are within
    $0, \dots, m-1$.
  - $| \gamma(z) | \leq m-1$.

## Practical Application

### Modern Relevance

- The principles behind Park's RNG are still relevant today, especially in
  systems where 32-bit arithmetic is common, such as embedded controllers
  (e.g., Raspberry Pi, Arduino).

### Adjustments for 64-bit Systems

- The constants and implementations can be scaled to accommodate 64-bit systems
  by adjusting the modulus $m$ to $2^{63} - 1$ and recalculating the dependent
  variables accordingly.

### Critique and Limitations
   - The Lehmer RNG’s weak statistical properties are inherent in its design, limiting its effectiveness as a generator of “true” random sequences.
   - The generator’s deterministic nature means that the sequence is not truly random, and improper parameters can lead to non-full-period generation.
   - The method's simplicity makes it appealing but requires careful attention to the choice of modulus and multiplier to avoid pitfalls.

## Conclusion

The study of Lehmer's RNG and Park's implementation reveals the importance of
mathematical rigor in RNG design. While historical constraints shaped the
original design, the underlying principles continue to inform modern
applications, particularly in constrained environments.

### Future Work
   - While the Lehmer RNG is a valid method, it is crucial to understand its limitations and ensure that the chosen parameters meet the required criteria.
   - Future exploration might include investigating alternative methods like the Mersenne Twister, which addresses some of the shortcomings of the Lehmer RNG.
