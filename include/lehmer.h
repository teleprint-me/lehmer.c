/**
 * Copyright © 2024 Austin Berrio
 *
 * @file include/lehmer.h
 *
 * @brief A 32-bit implementation of the Lehmer Random Number Generator (LCG)
 * in pure C
 *
 * The Lehmer Random Number Generator (LCG) is a simple yet elegant method
 * based on the equation f(z) = az \mod m. This implementation provides a
 * 32-bit version of the Lehmer LCG in pure C.
 *
 * @ref Random number generators: good ones are hard to find
 * @cite https://dl.acm.org/doi/10.1145/63039.63042
 * @cite https://www.cs.wm.edu/~va/software/park/park.html
 *
 * @ref Discrete Mathematics
 * @cite 1.2 Binomial Coefficients for definitions
 * @cite 5.1 Generating Functions for details
 *
 * @ref Introduction to Probability and Mathematical Statistics
 * @cite 9.3: The Bernoulli and Binomial Random Variables
 *
 * @note Development guidelines
 * - Keep the interface clean and transparent.
 * - Keep the implementation simple and to the point.
 * - Do not obfuscate user interaction.
 * - Encapsulate critical aspects of the interface.
 * - Hide complexity if and only if reasonably possible.
 *
 * Useful for autocomplete and mnemonic (easy to remember or ref)
 *
 * - (prefix)_(group)_(v.|adj.|n.)
 *
 * where prefix = lehmer, group = state, and verb = create
 * yields lehmer_state_create
 *
 * @todo Variates are a work in progress.
 *
 * @note new issue arises because each of the variates requires a generator as
 * well.
 */

#ifndef LEHMER_H
#define LEHMER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * @param LEHMER_MODULUS Mersenne prime number used as modulus (2^31 - 1).
 *
 * m: A large Mersenne prime number appropriate for a given bit width.
 *
 * @note Must be a mersenne prime number.
 * - 32-bit: 2^31 - 1 = 2147483647
 * - 64-bit: 2^61 - 1 = 2305843009213693951
 *
 * @note Originally chosen for the IBM System/360 to avoid overflow errors in
 * 32-bit integer arithmetic. This choice remains relevant for modern embedded
 * systems with similar constraints.
 */
#define LEHMER_MODULUS 2147483647

/**
 * @param LEHMER_MULTIPLIER Prime number used to scale the seed.
 *
 * a: A smaller prime number in the range 2, ..., m - 1
 *
 * @note Must be a prime number.
 *
 * @note The multiplier was specifically selected to prevent overflow errors on
 * hardware with 32-bit integer limitations. Despite being dated, this choice
 * is still effective in modern contexts where 32-bit constraints apply.
 *
 * @warning 16807 was originally chosen for safe 32-bit arithmetic.
 */
#define LEHMER_MULTIPLIER 48271

/**
 * @param LEHMER_JUMP Prime number used as a multiplier to "jump" scale seeds.
 *
 * @note The multiplier must be a prime number.
 */
#define LEHMER_JUMP 22937

/**
 * @param LEHMER_SIZE Default allocation size for a given number of seeds.
 */
#define LEHMER_SIZE 256

/**
 * @param LEHMER_SEED Default seed value.
 *
 * z: A integer value in the range 1, ..., m - 1
 *
 * @note May be any value within the defined range.
 */
#define LEHMER_SEED 123456789

/**
 * @param LEHMER_QUOTIENT Result of dividing LEHMER_MODULUS by
 * LEHMER_MULTIPLIER. Useful for range normalization in Lehmer-based
 * calculations.
 */
#define LEHMER_QUOTIENT ((LEHMER_MODULUS) / (LEHMER_MULTIPLIER))

/**
 * @param LEHMER_REMAINDER Result of modulus operation between LEHMER_MODULUS
 * and LEHMER_MULTIPLIER. Important for handling overflows and adjusting
 * outputs.
 */
#define LEHMER_REMAINDER ((LEHMER_MODULUS) % (LEHMER_MULTIPLIER))

/**
 * @brief Structure representing the state of the Lehmer Random Number
 * Generator (LCG RNG).
 *
 * The LehmerState structure contains the internal state of the LCG RNG,
 * including the seed, sequence of generated integers, number of values to
 * generate, and the current position in the sequence. This structure is used
 * to manage the state of the RNG and facilitate the generation of random
 * numbers.
 *
 * @param seed The initial seed used to start the sequence.
 * @param sequence The sequence of generated integers.
 * @param length The number of values to generate.
 * @param position The current position in the sequence.
 */
typedef struct LehmerState {
    int32_t seed; // The initial seed used to start the sequence
    int32_t* sequence; // The sequence of generated integers
    uint32_t length; // The number of values to generate
    uint32_t position; // The current position in the sequence
} lehmer_state_t;

/**
 * @brief Callback function for generating new seeds in the Lehmer RNG.
 *
 * @param state The current state object.
 */
typedef int32_t (*lehmer_generate_t)(int32_t);

// Lehmer state management

/**
 * @brief Create and initialize the Lehmer RNG state with dynamic seed
 * generation.
 *
 * @param seed The initial seed value.
 * @param length The number of values to generate (upper limit).
 *
 * @return A newly allocated and initialized Lehmer RNG state object or NULL if
 * memory allocation fails.
 */
lehmer_state_t* lehmer_state_create(int32_t seed, uint32_t length);

/**
 * @brief Free the allocated memory for the Lehmer RNG state object.
 *
 * @param state The Lehmer RNG state object to be freed.
 */
void lehmer_state_free(lehmer_state_t* state);

/**
 * @brief Print the Lehmer RNG state object.
 *
 * @param state The Lehmer RNG state object to be printed.
 */
void lehmer_state_print(lehmer_state_t* state);

// Lehmer seed management

/**
 * @brief Sets the initial seed used to start the sequence.
 *
 * @param state The Lehmer RNG state object.
 * @param seed The seed to set.
 */
void lehmer_seed_set(lehmer_state_t* state, int32_t seed);

/**
 * @brief Retrieves the initial seed used to start the sequence.
 *
 * @param state The Lehmer RNG state object.
 *
 * @return The initial seed used to start the sequence.
 */
int32_t lehmer_seed_get(lehmer_state_t* state);

// Lehmer position management

/**
 * @brief Set the current position in the sequence.
 *
 * @param state Pointer to the Lehmer state structure.
 * @param position New position to set.
 *
 * @note handles overflow with modulus.
 */
void lehmer_position_set(lehmer_state_t* state, uint32_t position);

/**
 * @brief Move to the next position in the sequence.
 *
 * @param state Pointer to the Lehmer state structure.
 *
 * @note handles overflow with modulus.
 */
void lehmer_position_next(lehmer_state_t* state);

/**
 * @brief Move to the previous position in the sequence.
 *
 * @param state Pointer to the Lehmer state structure.
 *
 * @note handles underflow with modulus.
 */
void lehmer_position_previous(lehmer_state_t* state);

// Lehmer sequence management

/**
 * @brief Sets the next value in the sequence.
 *
 * @param state The Lehmer RNG state object.
 * @param seed The seed to set in the sequence.
 */
void lehmer_sequence_set(lehmer_state_t* state, int32_t seed);

/**
 * @brief Retrieves the current value in the sequence.
 *
 * @param state The Lehmer RNG state object.
 *
 * @return The current value in the sequence.
 */
int32_t lehmer_sequence_get(lehmer_state_t* state);

// Lehmer seed normalization

/**
 * @brief Normalizes a seed to a float in the range 0.0 to 1.0
 *
 * @param state The Lehmer RNG state object.
 *
 * @return The normalized seed as a float in the range 0.0 to 1.0
 */
float lehmer_seed_normalize_to_float(int32_t seed);

/**
 * @brief Normalizes a seed to an integer in the range 0 to m - 1
 *
 * @param seed The seed to normalize.
 * @param modulus The modulus to use for normalization.
 *
 * @return The normalized seed as an integer in the range 0 to m - 1
 */
int32_t lehmer_seed_normalize_to_int(int32_t seed, uint32_t modulus);

// Lehmer seed generation

/**
 * @brief The Lehmer Random Number Generator is a simple yet elegant method
 * based on the equation f(z) = az \mod m.
 *
 * The core of Lehmer’s RNG is defined by the iterative equation
 *
 * z_{n+1} = \f(z_n), where \f(z) = a \cdot z \mod m
 *
 * @param[in] seed An integer representing the current seed.
 *
 * @return An integer representing the next seed in the sequence.
 */
int32_t lehmer_generate_modulo(int32_t seed);

/**
 * @note Intermediate results are bounded by m - 1
 * \f(z) = \gamma(z) + m \cdot \delta(z)
 */

/**
 * @brief Implementation of the gamma function for the Lehmer LCG PRNG
 *
 * This function calculates a scaled and shifted version of the original Lehmer
 * sequence, adjusting the range. It is an essential part of the binding
 * function for the Lehmer LCG PRNG.
 *
 * \gamma(z) = a \cdot (z \mod q) - r \cdot (z \div q)
 *
 * @param[in] seed An integer representing the current seed.
 *
 * @return An integer representing the next seed in the sequence.
 */
int32_t lehmer_generate_gamma(int32_t seed);

/**
 * @brief Implementation of the delta function for the Lehmer LCG PRNG
 *
 * This function calculates the delta value for the Lehmer LCG PRNG, which
 * might serve as an error correction term for the seed produced by the gamma
 * function. The implementation is based on the hypothesis that delta depends
 * on the same variables as gamma and is computed in a way that tries to
 * minimize the error introduced in the seed.
 *
 * \delta(z) = (z \div q) - (a \cdot z \div m)
 *
 * @param[in] seed An integer representing the current seed.
 *
 * @return An integer representing the next seed in the sequence.
 */
int32_t lehmer_generate_delta(int32_t seed);

/**
 * @brief The Lehmer Random Number Generator with a jump multiplier is a
 * variation of the original Lehmer’s RNG, which aims to further reduce
 * correlation between consecutive values.
 *
 * The core of the Lehmer LCG PRNG with a jump multiplier is defined by the
 * iterative equation
 *
 * \gamma(z) = a \cdot (z \mod q) - r \cdot (z \div q)
 *
 * @param[in] seed An integer representing the current seed.
 *
 * @return An integer representing the next seed in the sequence.
 */
int32_t lehmer_generate_jump(int32_t seed);

// Generalized Lehmer sequence generator

/**
 * @brief Generate a sequence of seeds using a specified generation function
 *
 * @param state The Lehmer RNG state object
 * @param generator The Lehmer RNG generation function to use
 * @param seed The initial seed value
 */
void lehmer_generate(
    lehmer_state_t* state, lehmer_generate_t generator, int32_t seed
);

// Generates a sequence using the current time as seed (non-deterministic)

/**
 * @brief Generate a sequence of seeds using the current time as the seed
 * (non-deterministic)
 *
 * @param state The Lehmer RNG state object
 * @param generator The Lehmer RNG generation function to use
 */
void lehmer_generate_time(lehmer_state_t* state, lehmer_generate_t generator);

/**
 * @brief Regenerate a new sequence of seeds based on the currently selected
 * seed.
 *
 * @param state The Lehmer RNG state object.
 * @param generator The Lehmer RNG generation function to use.
 */
void lehmer_regenerate(lehmer_state_t* state, lehmer_generate_t generator);

// Lehmer random number generators

/**
 * @brief Generate a random number using the modulo approach.
 *
 * @param state Pointer to the Lehmer state structure.
 *
 * @return Random float between 0 and 1.
 */
float lehmer_random_modulo(lehmer_state_t* state);
float lehmer_random_gamma(lehmer_state_t* state);
float lehmer_random_jump(lehmer_state_t* state);
float lehmer_random_delta(lehmer_state_t* state);

/**
 * @brief Generate a random number using a custom generator.
 *
 * @param state Pointer to the Lehmer state structure.
 * @param generator Function pointer to the generator method (e.g., modulo,
 * gamma).
 *
 * @return Random float between 0 and 1.
 */
float lehmer_random(lehmer_state_t* state, lehmer_generate_t generator);

// Lehmer probability functions

/**
 * @brief Checks if the provided probability is within the valid range
 *
 * @param p The probability to check
 *
 * @return true if the probability is within the valid range (0.0 < p < 1.0),
 *         false otherwise
 */
static inline bool lehmer_is_valid_probability(float p);

/**
 * @brief Generates a Bernoulli random variable with parameter p
 * @note Use 0.0 < p < 1.0
 *
 * The Bernoulli random variable X takes on the values 0 or 1 with
 * probabilities 1 - p and p, respectively.
 *
 * @param state The Lehmer RNG state object
 * @param p The probability of generating a 1 (0 < p < 1)
 *
 * @return 1 if the generated value is 1, 0 otherwise
 */
int32_t lehmer_bernoulli(lehmer_state_t* state, float p);

/**
 * @brief Generates a Binomial random variable with parameters n and p
 * @note Use n > 0 and 0.0 < p < 1.0
 *
 * The Binomial random variable X takes on the values 0, 1, 2, ..., n with
 * probabilities given by the binomial probability mass function.
 *
 * @param state The Lehmer RNG state object
 * @param n The number of trials
 * @param p The probability of success in each trial (0 < p < 1)
 *
 * @return A value between 0 and n inclusive, representing the number of
 * successes in n independent Bernoulli trials with probability p of success
 */
int32_t lehmer_binomial(lehmer_state_t* state, uint32_t n, float p);

#endif // LEHMER_H
