# Modular Arithmetic in Lehmer RNG

## Introduction
Modular arithmetic is a fundamental concept in the design of the Lehmer Random Number Generator (RNG) in our project. By applying a modulus operation, we can bound the generated random numbers within a fixed range and ensure that sequence positions wrap around predictably when they exceed their limits.

This document provides an in-depth exploration of modular arithmetic as it is applied in the Lehmer RNG, covering essential concepts such as signed and unsigned integer behavior, overflow, and underflow.

## Building Intuition with Division and Modulus
Modulus arithmetic can be challenging to grasp at first, especially when dealing with negative numbers or boundary conditions in C. However, by understanding the fundamental concept of division and the remainder, we can develop a solid intuition for modulus arithmetic.

Let's start by reviewing the **division** concept:

### Division with Remainder:
For any two numbers `a` and `d`, you can write the division as:

$$a = d \times q + r$$

Where:
- `q` is the quotient (how many times `d` divides into `a`),
- `r` is the remainder (what's left over after `d` is subtracted from `a` as many times as possible).

The remainder `r` is what modulus is concerned with. So, when we say:
$$a \mod d = r$$

It means we're just focusing on the remainder when dividing `a` by `d`, ignoring the quotient.

Let’s go over some examples to illustrate the concept:

## Examples
Modular arithmetic can be a bit tricky, especially when dealing with negative numbers or boundary conditions in C. To help build intuition, let's go over some examples to illustrate the concept:

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
Modular arithmetic is fascinating because of how it "wraps" numbers. If we 'overflow' or 'underflow', we simply 'wrap' around due to how the remainder works out.

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

Now, let’s explore this concept further with an example:

### Example: Position Management in C
Consider a sequence with a boundary of 256 values. When dealing with a position that can decrement below 0, the modulus operation ensures that the position "wraps" back to the upper bound.

In C, a common example would look like this:

```c
position = (position - 1) % boundary;
```

Where:
- `position` starts at 0,
- `boundary` is 256 (the maximum value for an 8-bit integer).

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
The modulus operation, denoted as `n % d`, returns the remainder when `n` is divided by `d`. This operation is crucial in many programming contexts, such as ensuring values remain within a certain range or handling cyclic behavior.

#### Signed and Unsigned Integers
In C, the behavior of signed and unsigned integers differs when it comes to overflow and underflow:

- **Signed Integers:** When a signed integer overflows, it wraps around to the other end of its value range. This behavior is predictable and can be thought of as a "safe" overflow.
- **Unsigned Integers:** Unsigned integers do not wrap around on overflow. Instead, the value can overflow into other memory or behave unexpectedly.

#### Overflow and Underflow
- **Overflow:** When a signed integer exceeds its maximum value, it wraps around to the minimum possible value. In contrast, an unsigned integer may produce unexpected results as it does not have a negative range.
- **Underflow:** For signed integers, underflow behaves similarly to overflow but in the opposite direction. The value wraps around to the maximum possible value.

## Practical Use of Modulus in Lehmer RNG

In the Lehmer RNG, we use modular arithmetic to handle sequence generation and boundary enforcement. The modulus ensures that positions in the sequence remain within valid bounds, and overflows or underflows do not result in undefined behavior.

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
