# Lehmer-Park RNG Implementation and Analysis

## Overview

This document provides a detailed analysis and explanation of the key aspects
related to the Lehmer random number generator (RNG), with a specific focus on
the constants and mathematical foundations discussed in the paper, _Random
Number Generators: Good Ones Are Hard to Find_ by Stephen Park.

## Key Concepts

### Lehmer's Algorithm

The Lehmer Random Number Generator is a simple yet elegant method based on the
equation $f(z) = az \mod m$.

- **Equation**: The core of Lehmer’s RNG is defined by the iterative equation
  $z_{n+1} = f(z_n)$, where $f(z) = a \cdot z \mod m$.
- **Parameters**:
  - **Modulus (m)**: A large prime integer, typically chosen for its
    mathematical properties.
  - **Multiplier (a)**: An integer in the range $2, \dots, m-1$, chosen to
    satisfy specific criteria.
- **Normalization**: The sequence is normalized to the unit interval by
  dividing by the modulus to produce the sequence $u_n = \frac{z_n}{m}$.

### Historical Context and Choices

- **Multiplier (a = 16807)**: This value was chosen due to hardware constraints
  at the time of Park's writing, specifically for systems with a 32-bit integer
  limit.
- **Modulus (m = 2147483647)**: A Mersenne prime, $2^{31} - 1$, selected for
  its suitability in achieving the full period requirement and ease of
  computation on 32-bit systems.

### Full Period Generating Function

- **Definition**: A function $f(z) = a \cdot z \mod m$ is a full-period
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

- **Bounded Results**: The equation $f(z) = \gamma(z) + m \cdot \delta(z)$
  ensures that all intermediate results are bounded by $m - 1$. Here:
  - $\gamma(z) = a \cdot (z \mod q) - r \cdot (z \div q)$
  - $\delta(z) = (z \div q) - (a \cdot z \div m)$
- **Significance of Constants**:
  - $\delta(z)$ is either 0 or 1.
  - Both $a \cdot (z \mod q)$ and $r \cdot (z \div q)$ are within the range
    $0, \dots, m-1$.
  - $\lvert \gamma(z) \rvert \leq m-1$.

## Deep Analysis

### Core Components of Lehmer’s RNG:

1. **Lehmer’s Core Equation**:

$$z\_{n+1} = f(z_n) = a \cdot z_n \mod m$$

- **`a`**: Multiplier (an integer)
- **`m`**: Modulus (a large prime integer)
- **`z_n`**: Seed or current value in the sequence

2. **Normalization**:

$$u_n = \frac{z_n}{m}$$

- This converts the raw output into the [0, 1) interval.

### Functions in Detail:

1. **Binding Function (`f`)**:

$$f(z) = \gamma(z) + m \cdot \delta(z)$$

- The binding function combines the results of `gamma` and `delta`.

2. **Gamma Function (`γ`)**:

$$\gamma(z) = ((a \cdot z) \mod q) - \left(\frac{r \cdot z}{q}\right)$$

- **`a * z`**: Part of the Lehmer iteration
- **`q`**: A modulus, different from `m`. Typically a smaller modulus used to
  limit the range of the intermediate value.
- **`r`**: An integer that modifies the quotient part of the gamma function.
  It’s not necessarily the remainder but serves as a scaling factor for the
  second term.

**Interpreting `r * z / q`**:

- `r` is the remainder of `m` and `a`.
- `q` is the ratio of `m` and `a` and is likely chosen to control the range of
  values in the gamma function.
- Essentially, `γ(z)` computes a scaled and shifted version of the original
  Lehmer sequence, adjusting the range.

3. **Delta Function (`δ`)**:

$$\delta(z) = \frac{z}{q} - \left(\frac{a \cdot z}{m}\right)$$

- **`z / q`**: Normalization or scaling term.
- **`a * z / m`**: Scaled by the multiplier and modulus, which might adjust the
  overall range or distribution.

**Significance of `δ`**:

- The delta function provides a way to normalize or adjust values coming from
  the gamma function. By subtracting these terms, it might smooth out the
  distribution or adjust the bias introduced by the gamma function.

### Analysis and Insights:

1. **Seed Usage**:

   - The seed (`z`) undergoes transformations through `gamma` and `delta`,
     which modify its distribution and range.

2. **Normalization Effects**:

   - The gamma function adjusts the raw output, and the delta function further
     normalizes it, producing values within a desired distribution range.

3. **Constants (`a`, `m`, `q`, `r`)**:

   - Constants play crucial roles in shaping the output. They influence the
     distribution, range, and normalization behavior.

4. **Refinement**:
   - To improve the implementation, ensure that the values of `q`, `r`, and
     other constants are chosen appropriately to balance the distribution. The
     behavior of `gamma` and `delta` should be validated against theoretical
     expectations and empirical results.

### Testing and Verification:

1. **Check Ranges and Distributions**:

   - Verify if the values produced by `gamma` and `delta` are within expected
     ranges and show desired statistical properties.

2. **Adjust Parameters**:

   - Experiment with different values for `q`, `r`, and other constants to see
     their impact on the distribution.

3. **Compare with Theory**:
   - Ensure that the outputs align with theoretical expectations for randomness
     and distribution.

## Practical Application

### Modern Relevance

- The principles behind Park's RNG are still relevant today, especially in
  systems where 32-bit arithmetic is common, such as embedded controllers
  (e.g., Raspberry Pi, Arduino).

### Adjustments for 64-bit Systems

- The constants and implementations can be scaled to accommodate 64-bit systems
  by adjusting the modulus $m$ to $2^{61} - 1$ and recalculating the dependent
  variables accordingly.

### Critique and Limitations

- The Lehmer RNG’s weak statistical properties are inherent in its design,
  limiting its effectiveness as a generator of “true” random sequences.
- The generator’s deterministic nature means that the sequence is not truly
  random, and improper parameters can lead to non-full-period generation.
- The method's simplicity makes it appealing, but it requires careful attention
  to the choice of modulus and multiplier to avoid pitfalls.
- The generator is sensitive to minor adjustments and may become unbounded,
  indicating fragility in the method.

## Conclusion

The study of Lehmer's RNG and Park's implementation reveals the importance of
mathematical rigor in RNG design. While historical constraints shaped the
original design, the underlying principles continue to inform modern
applications, particularly in constrained environments.

### Future Work

- While the Lehmer RNG is a valid method, it is crucial to understand its
  limitations and ensure that the chosen parameters meet the required criteria.
- Future exploration might include investigating alternative methods like the
  Mersenne Twister, which addresses some of the shortcomings of the Lehmer RNG.
