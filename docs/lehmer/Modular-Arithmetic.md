# Modular Arithmetic in Lehmer RNG

## Introduction
Modular arithmetic is a fundamental concept in the design of the Lehmer Random Number Generator (RNG) in our project. By applying a modulus operation, we can bound the generated random numbers within a fixed range and ensure that sequence positions wrap around predictably when they exceed their limits.

This document provides an in-depth exploration of modular arithmetic as it is applied in the Lehmer RNG, covering essential concepts such as signed and unsigned integer behavior, overflow, and underflow.

## Building Intuition with Division and Modulus
Modulus arithmetic can be challenging to grasp at first, especially when dealing with negative numbers or boundary conditions in C. However, by understanding the fundamental concept of division and the remainder, we can develop a solid intuition for modulus arithmetic.

Let's start by reviewing the **division** concept:

$$\text{quotient} = \frac{\text{dividend}}{\text{divisor}}$$

or

$$\text{quotient} = \frac{\text{numerator}}{\text{denominator}}$$

Both are equivalent. The latter is nicer than the former because we can use the initial letters $q$ (quotient), $n$ (numerator), and $d$ (denominator), respectively. This allows us to mitigate any conflicts in variable names.

### Division with Remainder:
Modular arithmetic is based on the concept of division with remainder. When you divide an integer $n$ by another integer $d$, you can write the division as:

$$n = d \times q + r$$

Where:
- $n$ is the numerator (the number divided into)
- $d$ is the denominator (the number divided by)
- $q$ is the quotient (how many times $d$ divides into $n$),
- $r$ is the remainder (what's left over after $d$ is subtracted from $n$ as many times as possible).

The modulus operation directly gives us the remainder $r$. Mathematically, this is written as:

$$n \mod d = r$$

This expression means we're just focusing on the remainder when dividing $n$ by $d$, ignoring the quotient. This operation returns the remainder $r$ when $n$ is divided by $d$, and $r$ always satisfies the condition where $r$ is greater than or equal to $0$ and is less than the absolute value of $d$.

This can be written mathematically as:

$$0 \leq r < |d|$$

We can define this as for all $r$ within the set of integers such that the absolute value of $|d|$ is positive.

The more compact definition of this might look like the following:

$$\forall r \in \mathbb{Z}$$

We can expand this expression by defining $r$ as a *specific subset* of integers constrained by the modulus operation:

$$r \in \{0, 1, 2, \dots, |d| - 1\}$$

This emphasizes that $r$ is not just any integer and that it remains within the bounds of the modulus result.

This allows us to say that $n \mod d$ is congruent with $r$:

$$n \mod d \equiv r$$

To say that this is congruent means we define the expression as being two numbers which have the same remainder when divided by a third number. For example, 10 and 3 are congruent when the modulus is 1.

## Examples
Modular arithmetic can be a bit tricky, especially when dealing with negative numbers or boundary conditions in C. To help build intuition, let's go over some examples to illustrate the concept.

### Example 1: $10 \div 5$
$$10 = 5 \times 2 + 0$$

The remainder $r = 0$ because 10 is evenly divisible by 5. Therefore:

$$10 \mod 5 = 0$$

### Example 2: $10 \div 3$
Here’s where it gets interesting:

$$10 = 3 \times 3 + 1$$

The remainder $r = 1$ because 10 isn’t evenly divisible by 3; we can subtract 9 ($3 \times 3$) from 10, and we're left with 1. So:

$$10 \mod 3 = 1$$

The **modulus** operation effectively gives you the leftover portion after you subtract as many multiples of the divisor as possible.

### Extension to Modular Arithmetic
Modular arithmetic is fascinating because of how it "wraps" numbers. If we "overflow" or "underflow", we simply "wrap" around due to how the remainder works out.

This wrapping behavior is a core property of modulus, and it’s why modulus is so useful in applications like random number generation, hashing, or circular buffers.

### Example 3: Negative Values and Modulus
When working with **negative numbers** in modulus, things can get confusing. For example:

$$-1 \mod 3 = 2$$

Why? Because:

$$-1 = 3 \times (-1) + 2$$

In this case, the remainder has to be non-negative, so the modulus operation gives us 2. Essentially, we’re "wrapping" around the range defined by the divisor (3) and ensuring the result is positive.

## Modulus and Overflow/Underflow
In programming, modulus arithmetic is valuable because it constrains values within a certain range. When performing an operation like:

$$a \mod m$$

the result is always between $0$ and $m - 1$, no matter how large or small `a` is. This makes modulus an ideal tool for handling cyclic behavior, such as wrapping sequence positions in an RNG or managing circular buffers.

### Position Management
Consider traversing a sequence of values with an upper bound of 256 values. The lower boundary is 0 and the upper boundary is 256.

We keep track of where we are by defining a **position**. If the position is **unbounded**, then we may traverse undefined values. If the position is **bounded**, then we may traverse defined values.

By binding the position to the sequence boundaries, we remain within a range of known values.

For example, when dealing with a position that can decrement below 0, the modulus operation ensures that the position "wraps" back to the upper bound.

When programming, we would begin by defining the position and boundary:

```c
const unsigned int boundary = 256;
signed int position = 0;
```

Where:
- `position` starts at 0,
- `boundary` is 256 (the maximum value for an 8-bit integer).

```c
position = (position - 1) % boundary;
```

If `position` is decremented below 0, the modulus operation wraps it back into the valid range `[0, boundary - 1]`.

#### Stepping Through the Example:
Let’s mathematically step through what happens:

1. **Start with position = 0**  
   Subtract 1:  
   $$\text{position} = 0 - 1 = -1$$  
   Apply modulus:  
   $$-1 \mod 256 = 255$$  
   Result: `position` becomes 255, effectively wrapping around to the other end of the range.

2. **Decrement again from 255**  
   Subtract 1:  
   $$\text{position} = 255 - 1 = 254$$  
   Apply modulus:  
   $$254 \mod 256 = 254$$  
   No wrapping is needed in this case.

This process continues, ensuring that even as `position` decrements beyond the lower boundary, it wraps around correctly due to the modulus operation. This predictable behavior allows for safe navigation within the sequence.

### Modulus as a Bounding Mechanism
Thinking of modulus as a **bounding mechanism** helps to better understand its usefulness in certain contexts:

- It keeps values **within a specified range**, regardless of the size of the input.
- It’s ideal for managing **cyclic structures**, such as wrapping around the ends of arrays, managing indices in circular buffers, or ensuring random number outputs are within a defined range.

Modulus arithmetic may initially seem unintuitive because it deals with wrapping values, which isn't immediately obvious when considering division. However, the key advantage of modulus lies in its ability to **constrain values** within a specific range, making it an essential tool for many algorithms and applications.

In the context of the Lehmer generator, the modulus ensures that the seed or position **remains within valid bounds** (i.e., within the range of the sequence or array) even when incrementing or decrementing beyond the limits.

Modular arithmetic isn’t just division with a twist—it’s also a powerful tool for handling cyclic behavior, which is why it appears in areas like number theory, cryptography, and computer science algorithms.

### Modulus Operation

#### Signed and Unsigned Integers
In C, the behavior of signed and unsigned integers differs when it comes to overflow and underflow:

- **Signed Integers:** When a signed integer overflows, it wraps around to the other end of its value range. This behavior is predictable and can be thought of as a "safe" overflow.
- **Unsigned Integers:** Unsigned integers do not wrap around on overflow. Instead, the value can overflow into adjacent memory resulting in undefined behavior.

#### Overflow and Underflow
- **Overflow:** When a signed integer exceeds its maximum value, it wraps around to the minimum possible value. In contrast, an unsigned integer may produce unexpected results as it does not have a negative range.
- **Underflow:** For signed integers, underflow behaves similarly to overflow but in the opposite direction. The value wraps around to the maximum possible value.

## Practical Use of Modulus in Lehmer RNG

In the Lehmer RNG, we use modular arithmetic to handle sequence generation and boundary enforcement. The modulus ensures that positions in the sequence remain within valid bounds, and overflows or underflows do not result in undefined behavior.

```c
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
```

### Example

```c
signed int position = 0;
const unsigned int boundary = 256;

position = (position - 1) % boundary;
```

Here, the `position` variable is decremented, and the modulus operation ensures that it stays within the bounds defined by `boundary`. If the position underflows, it wraps around to the maximum boundary value.

### Simplifying Expressions

In certain cases, you may see the following pattern:

```c
position = (position - 1 + boundary) % boundary;
```

This can be simplified to:

```c
position = (position - 1) % boundary;
```

Both expressions are equivalent because the modulus operation ensures the result stays within bounds, eliminating the need to explicitly add the boundary in most cases.

## Sign Behavior of Modulus

In modular arithmetic, the sign of the divisor (denoted as `m`) plays an important role in determining the sign of the remainder when performing the modulus operation. Understanding how this behavior works is crucial when using modular arithmetic in programming.

#### 1. **Positive Divisor (`m > 0`)**
When the divisor `m` is positive, the modulus operation `a % m` always returns a remainder within the range `[0, m - 1]`. This behavior holds regardless of whether `a` (the number being divided) is positive or negative.

- If `a` is positive, the remainder is naturally between `0` and `m - 1`.
- If `a` is negative, the modulus "wraps" the result back into the positive range.

**Examples:**
```python
>>> a = 5
>>> m = 3
>>> a % m
2  # Remainder is within [0, 2]

>>> a = -5
>>> m = 3
>>> a % m
1  # Despite being negative, the remainder wraps back to the positive range
```

#### 2. **Negative Divisor (`m < 0`)**
When the divisor `m` is negative, the remainder will always fall within the range `[m, -1]`. This is because the sign of `m` determines the wrapping behavior of the modulus operation.

- If `a` is positive, the remainder is wrapped to be negative.
- If `a` is negative, the modulus operation still respects the negative divisor and provides a remainder within the range `[m, -1]`.

**Examples:**
```python
>>> a = 5
>>> m = -3
>>> a % m
-1  # Result is within [-3, -1]

>>> a = -5
>>> m = -3
>>> a % m
-2  # Result remains negative
```

#### 3. **Ensuring a Positive Remainder**
In some situations, you might want to ensure that the remainder is always positive, even when `a` or `m` is negative. To achieve this, you can adjust the modulus operation by using the absolute value of `m`:

```python
>>> a = -5
>>> m = -3
>>> (a % abs(m)) 
1  # Using the absolute value of m ensures a positive remainder
```

This technique is useful when working in contexts where positive remainders are expected or necessary, such as when indexing arrays or managing cyclic data structures.

Note that nesting the modulus to the result has no effect:

```python
>>> a = -5
>>> m = 3
>>> (a % m + m) % m
1  # Has no effect
>>> (a % m + m) % m == a % m
True
```

Adding the modulus to the result still results in no effect:

```python
>>> (a + m) % m == a % m
True
```

We can see that they remain equivalent throughout. The expressions retain their equivalency, regardless of structure.

#### 4. **Range of Remainders**
In both cases, the remainder will always fall within the range:
- For `m > 0`, the remainder is within `[0, m - 1]`.
- For `m < 0`, the remainder is within `[m, -1]`.

The modulus operation essentially binds values to these ranges, making it a helpful tool for cyclic or wrapping operations.

**Examples with Larger Values:**
```python
>>> m = 256
>>> a = 300
>>> a % m
44  # Positive range [0, 255]

>>> m = -256
>>> a = -300
>>> a % m
-44  # Negative range [-256, -1]
```

In summary, the sign of the divisor determines the range in which the remainder falls. A positive divisor results in a positive remainder, while a negative divisor leads to a negative remainder. Using the absolute value of the divisor can help ensure that the remainder is always positive, which is useful in certain applications.

## The Lehmer RNG
The Lehmer Random Number Generator (RNG) is a classic example of a **Linear Congruential Generator (LCG)**, widely used to produce pseudo-random sequences. It extends the basic principles of modular arithmetic by introducing two additional key concepts: **scalar values** and **recursive scaling**.

### Formal Definition
The Lehmer RNG is defined by the equation:

$$f(z) = (a \times z) \mod m$$

Where:
- $m$ is a Mersenne prime (a prime number of the form $2^p - 1$),
- $a$ is a prime multiplier (a number divisible only by $1$ and itself),
- $z$ is the initial value (known as the **seed**).

### Scalar Values
In the Lehmer RNG, the **scalar value** is represented by $z$, which serves as the seed. This seed initializes the process and is crucial in generating the sequence. Once defined, the seed $z$ is used as the input to the modulus operation:

$$z_{n+1} = (a \times z_n) \mod m$$

Here, $z_{n+1}$ is the next value in the sequence, derived by scaling the current seed $z_n$ by the multiplier $a$, followed by a modulus with $m$. The seed value evolves through the sequence, confined within the range $[0, m - 1]$.

### Recursive Scaling
One of the key innovations of the Lehmer RNG is **recursive scaling**. This means that the output of each iteration is used as the input for the next iteration. Specifically, the result from one step (the new seed $z_{n+1}$) is fed back into the equation for the next iteration:

$$z_{n+2} = (a \times z_{n+1}) \mod m$$

This recursive process creates a sequence of pseudo-random values that depends entirely on the initial seed.

### Modular Arithmetic in the Lehmer RNG
The Lehmer RNG relies on modular arithmetic to keep the generated numbers within a specific range. The general modulus operation is defined as:

$$n \mod d = r$$

Where:
- $n$ is the input value,
- $d$ is the divisor,
- $r$ is the remainder.

In the Lehmer RNG context, the modulus operation takes the form:

$$(a \times z) \mod m = z$$

Where:
- $n = a \times z$ is the product of the multiplier and the seed,
- $d = m$ is the modulus (a Mersenne prime),
- $r = z$ is both the input and output seed.

### Seed Selection and Iteration
The selection of the initial seed $z_0$ is crucial, as it defines the starting point for the entire pseudo-random sequence. After each iteration, the output seed $z_{n+1}$ becomes the new input for the next round of calculations, recursively generating the sequence.

This iterative process means the remainder $r$, which is $z$, functions as both the output and input in each step. The choice of $a$ and $m$ helps ensure that the sequence retains reasonable statistical properties and achieves a long period before repeating itself.

#### Periodicity and Weak Properties
However, it's important to note that these statistical properties are inherently weak due to the periodic nature of the generator. **Periodicity** means that no matter how large the modulus $m$ is, the sequence will eventually repeat itself. Although this repetition may not be immediately apparent, it is inevitable and mathematically derivable. This upper limit defines the sequence’s period, which can introduce weaknesses when high-quality randomness or unpredictability is required.

### Function Definition
The function $f(z)$ can be defined as the relationship between the input seed and the output seed:

$$f(z) = (a \times z) \mod m$$

This defines a mapping from one seed value to the next, creating a deterministic number sequence - e.g. A pseudo-random number generated sequence.

## Conclusion

In conclusion, the use of modular arithmetic is a fundamental aspect of the Lehmer Random Number Generator. By understanding the properties of modular arithmetic, we can harness its power to create bounded and predictable sequences. This is crucial for applications that require cyclic behavior, such as random number generation, hashing, or circular buffers.

By normalizing the generated numbers to the set of integers $\mathbb{Z}$, we ensure that the results are always integers and can be easily bound within a specific range. This normalization makes it possible to work with cyclic structures like arrays and circular buffers effectively.

Furthermore, understanding the behavior of signed and unsigned integers, as well as the sign of the divisor, helps us to create more robust and efficient algorithms. With the knowledge gained in this document, you are now well-equipped to apply modular arithmetic in a variety of contexts and tackle real-world problems with confidence.

We hope this document has served as a valuable resource for you, and we encourage you to continue exploring the fascinating world of modular arithmetic and its applications. Happy coding!
