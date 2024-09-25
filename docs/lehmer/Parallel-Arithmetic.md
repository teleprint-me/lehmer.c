# Experimental Parallelized Number Generation

## Abstract
This document outlines experimental approaches to parallelizing number generation using a Lehmer RNG. The focus is on exploring techniques for CPU-bound parallelization that avoid proprietary hardware dependencies such as AVX, ARM, CUDA, ROCm, etc., and instead emphasize general-purpose methods that can be applied in common environments. Lehmer’s simplicity ($f(z) = a \cdot z \mod m$) makes it ideal for such experimentation, providing an easy-to-follow framework.

## Introduction
Random number generation (RNG) is a fundamental component in many domains, from simulations to cryptography. Traditional RNGs like the Lehmer generator, defined by $f(z) = a \cdot z \mod m$, generate a sequence of pseudo-random values, but they are inherently linear, with each output depending on the previous one. This creates a bottleneck for parallelization, especially in multi-threaded or distributed systems.

Our goal is to explore methods for parallelizing RNGs in a way that scales effectively without relying on specific hardware like GPUs. While skip-ahead techniques offer potential, they often introduce complexities that hinder their practicality. We start with Lehmer’s formula as the base and outline possible techniques for splitting the problem into parallelizable components.

## Problem Description
The challenge of parallelizing Lehmer RNG lies in its linear dependence. Each value $z_n$ relies on $z_{n-1}$, meaning that in a single-threaded context, the sequence can only advance one step at a time. In a multi-threaded environment, this dependency between steps constrains the independence of threads, forcing a sequential operation where only one thread can compute the next random number at a time.

### Key Issues in Multi-threading Number Generation

1. **Sequential Dependence**:  
   Each seed relies on the previous seed in the sequence. For example, to compute $z_n$, you must know $z_{n-1}$. This creates a strict chain of dependencies that prohibits fully independent thread execution.

2. **Unknown Initial Seed**:  
   The starting seed $z_0$ is critical for all future computations. In a single-threaded system, this is trivial, but in a multi-threaded or distributed context, each thread either needs access to the original seed or an independent seed, leading to synchronization and potential performance bottlenecks.

3. **Computation for Every Integer in Range**:  
   To maintain uniform randomness, each thread must compute for every integer in the possible range $[0, m-1]$. This means every potential output must be generated sequentially unless a clever parallelization strategy can be introduced.

4. **No Pre-existing Data**:  
   Without precomputed sequences or access to large tables of values, there is no easy way to "skip ahead" or compute intermediate values efficiently without iterating through all prior results. This lack of foresight in the sequence leads to tight coupling between steps.

### Exploring Parallelization: Jump-Ahead and Disjointed Sets

A technique that shows promise in overcoming some of these limitations is the **jump-ahead method**, which modifies the Lehmer equation to allow computation of a future value without computing all the intervening steps:

$$ z_{n+k} = z_n \times a^k \mod m $$

This jump-ahead formula allows us to bypass the strict $z_{n-1} \to z_n$ sequence and compute values multiple steps ahead in one calculation. However, it comes with trade-offs in terms of added complexity and increased computational overhead, as $a^k \mod m$ must be precomputed or calculated on demand.

Another potential approach involves **disjointed sets**, where we split the sequence into non-overlapping subsequences, with each thread responsible for its own independent chunk of the range $[0, m-1]$. This method would require either clever seeding or precomputing distinct starting points for each thread, ensuring that no thread’s output overlaps with another.

We will explore both of these techniques in more detail in the following sections, starting with the naive projection method for parallel computation.

## Naive Projection Approach
Our first approach to address this limitation is the "naive projection" method, where we attempt to calculate the next step in the sequence in parallel. The formula $z' = a \cdot z \mod m$ allows us to compute $z_{n+1}$, or a projection of the next value in parallel threads. While this offers a glimpse of potential parallelism, it doesn't entirely resolve the dependency issue since each thread still requires knowledge of the previous seed.

### Step-by-Step Naive Projection

1. **Threading for Single-Step Lookahead**: Each thread generates a single step of the RNG based on the current state of the seed.
   
2. **Result Synchronization**: The results from each thread are collected and synchronized, ensuring the correct sequence order is maintained across threads.

3. **Binding Output**: Using the modulus $m$, the results are bound to the expected range, ensuring that all outputs stay within the valid range $[0, m-1]$.

### Code Example

```python
import threading

# Define the Lehmer RNG function
def lehmer(seed, a, m):
    return (a * seed) % m

# Thread task to calculate the next value in the sequence
def generate(seed, multiplier, modulus):
    next_value = lehmer(seed, multiplier, modulus)
    print(f"Generated value: {next_value}")
    return next_value

# Parameters
seed = 1337
multiplier = 48271
modulus = 2147483647

# Thread setup
threads = []
for i in range(4):  # Example with 4 threads
    t = threading.Thread(target=generate, args=(seed, multiplier, modulus))
    threads.append(t)
    t.start()

# Wait for all threads to finish
for t in threads:
    t.join()
```

## Limitations and Observations
While the naive projection method offers a small window into parallelization, it quickly reveals the fundamental issue with Lehmer: the tight coupling between each step. Each thread can only compute the next step in isolation, and attempting to move beyond that would require knowing future seeds, defeating the purpose of a sequential RNG.

## Conclusion
Lehmer’s simplicity makes it a valuable case study in the exploration of parallelization techniques, but its linear nature presents significant challenges. While simple threading and single-step lookahead offer some degree of concurrency, more sophisticated approaches such as disjointed sets or skip-ahead techniques will be necessary for meaningful parallelization. Future work will focus on exploring these more complex techniques, as well as their practical implications for large-scale RNG tasks.
