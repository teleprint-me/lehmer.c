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

### Exploring Tables: Pre-computed Parallel Operations
Precomputing sequences of random numbers might offer a potential solution to parallelizing Lehmer RNG, but this approach brings its own set of limitations:

- **Memory Overhead**: Storing precomputed sequences, especially for large ranges $[0, m-1]$, would require a significant amount of memory. For larger values of $m$, this becomes impractical, especially when scaling to millions or billions of seeds.
  
- **Limited Flexibility**: Precomputed values limit the generator's flexibility, as any changes in seed, multiplier, or modulus would require regenerating the entire table, negating the initial benefits of parallelism.
  
- **Lookup Costs**: While precomputing may seem to offload the calculation, the cost of looking up precomputed sequences (especially with large tables) can outweigh the computational gains. The time spent on lookup operations, cache misses, and I/O can add overhead, especially when random access is required.

Thus, while precomputed tables might be useful in specific contexts (e.g., for very small seed ranges or highly optimized environments), their real-world applicability remains limited for general-purpose parallelization.

### Further Considerations

1. **Pipeline Parallelism**:  
   While Lehmer’s core sequence generation is inherently sequential, breaking the workload into a pipeline could offer some gains. For example, one thread could handle seed generation while another handles post-processing steps (e.g., normalization, boundary checks). While this doesn’t reduce the time complexity of generating the sequence, it could help make better use of CPU resources.

2. **Approximation or Lazy Evaluation**:  
   If exact randomness isn't required, it might be possible to approximate large chunks of the sequence or generate values only as needed (lazy evaluation). This would reduce unnecessary computation and memory usage but would come with trade-offs in statistical properties.

3. **GPU Acceleration**:  
   Offloading computation to the GPU could handle massive amounts of seed generation in parallel. While Lehmer’s sequential nature makes this challenging, simpler GPU operations (like modular arithmetic) could speed up batch generation of random numbers in environments where GPU resources are available. However, this moves away from the CPU-bound focus of this document.

## Initial Approach: A Naive Projection
The challenge of parallelizing the Lehmer RNG prompts a thought experiment that, while flawed, still helps to explore the space of possibilities. This concept involves treating the next value in the sequence as a kind of **projection**—an estimate of where the sequence is headed, even though the strict linear dependency of Lehmer’s RNG complicates matters.

Let’s start with the core formula:

$$z' = a \cdot z \mod m$$

This equation computes the next seed $z_{n+1}$ given the current seed $z_n$. In a single-threaded scenario, this simple recursion is straightforward but inherently linear. Each subsequent value depends on the previous one. However, thinking of $z'$ as a **naive projection** allows us to frame the process as "stepping forward" in time—albeit only one step at a time. This approach hints at limited forms of parallelism, where each thread works on its own independent projection.

### Stepping Forward in Time
Viewing $z'$ as the next step in a **forward projection** provides an opportunity to explore parallel computation. Suppose we attempt to compute multiple such projections simultaneously:

- $z'_1 = a \cdot z_1 \mod m$
- $z'_2 = a \cdot z_2 \mod m$
- $\dots$
- $z'_n = a \cdot z_n \mod m$

Each projection is essentially a parallel calculation of the next seed in the sequence, but the key limitation is that **each calculation still relies on its own initial seed**. We cannot compute more than one step ahead in a single sequence, as $z_{n+1}$ depends on $z_n$, and this sequential dependence fundamentally limits the degree of parallelism.

### A Limited View
This approach provides us with a limited form of parallelism that allows us to batch the computation of seeds for **different independent sequences**, but it doesn’t address the issue of **speeding up a single sequence**. In other words, parallelism is only achievable if we are generating multiple independent sequences rather than trying to accelerate the generation of one long sequence.

In this sense, the "naive projection" is not entirely fruitless, as it can parallelize the generation of multiple random number streams in certain applications. However, it does little to address the dependency constraints that arise when computing a single continuous sequence.

### Parallelism via Batching Independent Blocks of Sequences
While parallelism in a single sequence is constrained by its linear nature, there is potential in **batching independent sequences**:

- Thread 1 generates the next seed for sequence 1: $z'_1 = a \cdot z_1 \mod m$
- Thread 2 generates the next seed for sequence 2: $z'_2 = a \cdot z_2 \mod m$
- $\dots$
- Thread n generates the next seed for sequence n: $z'_n = a \cdot z_n \mod m$

In this way, we can compute seeds in parallel, provided that each thread works on an independent sequence. While this approach won’t speed up the generation of a single sequence, it could be highly beneficial in applications where multiple independent random number streams are required—such as Monte Carlo simulations or simulations where many distinct random sequences are needed.

### Dead Ends and Insights
While the naive projection method doesn’t solve the problem of parallelizing a single sequence, it serves to illuminate the limits of what can be parallelized. In fact, exploring this dead-end reaffirms the fundamental constraints imposed by sequential dependence and opens up new questions about where parallelism might be more effectively applied.

This exploration brings us back to the core issue: 

**how do we overcome the linear dependence between sequential seeds?**

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
