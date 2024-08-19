# lehmer.c

## Overview

Random Number Generators (RNGs) are foundational tools in numerous applications, including simulations, cryptography, games, and statistical sampling. The choice of RNG can significantly influence the performance and reliability of your projects. This repository aims to provide robust and efficient RNG implementations, with a focus on adaptability and maintainability.

### Types of RNGs:

1. **Linear Congruential Generator (LCG)**: A simple and efficient pseudorandom number generator that uses a recursive formula. While commonly used, LCGs can have weaker statistical properties compared to more advanced generators.
   
2. **Mersenne Twister**: Known for its excellent statistical properties, this RNG is widely used in scientific computing due to its ability to pass rigorous randomness tests.
   
3. **Discrete Fourier Transform (DFT) Generator**: A deterministic RNG that leverages complex arithmetic and trigonometric functions. It offers strong security and reliability, making it ideal for high-security applications like cryptography, though it can be computationally intensive.

## About `lehmer.c`

`lehmer.c` implements a Linear Congruential Generator in C, building upon the foundational work by Steven Park and Derrick Lehmer. This project aims to refine and extend the LCG, offering a versatile and robust tool for various downstream applications.

### Key Features:

- **Enhanced LCG Algorithm**: Optimized for modern use cases, offering improved performance and usability.
- **Modular Design**: Easily integrates with other projects, designed with maintainability and future extensions in mind.
- **Lightweight and Efficient**: Prioritizes performance, suitable for resource-constrained environments.

## Future Extensions

This repository is structured to potentially include other RNG implementations, such as the Mersenne Twister and Discrete Fourier Transform generators, providing a comprehensive suite of RNG tools in a single, easy-to-manage package.

## Usage

To use the `lehmer.c` LCG in your project, simply include the source file and headers. Future extensions will be seamlessly integrated to allow for more complex RNG needs.

## Dependencies

- **float_is_close.c**: This module is a dependency for precise floating-point comparisons within the RNG algorithms.

## References

### Mathematical Background:
- [Discrete Mathematics](https://math.libretexts.org/Bookshelves/Combinatorics_and_Discrete_Mathematics/Discrete_Mathematics_(Levin))

### Original Papers:
- [Random number generators: good ones are hard to find](https://dl.acm.org/doi/10.1145/63039.63042)
- [Mersenne Twister: A 623-dimensionally equidistributed uniform pseudo-random number generator](https://dl.acm.org/doi/10.1145/272991.272995)

### Original Source Code:
- [Steve Park's RNG Library](https://www.cs.wm.edu/~va/software/park/park.html)

## License
This project is licensed under the AGPL License - see the [LICENSE](LICENSE) file for details.
