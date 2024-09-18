#include <stdint.h>

#define LEHMER_MODULUS 2147483647 // mersenne prime number
#define LEHMER_MULTIPLIER 48271 // prime number
#define LEHMER_SEED 123456789 // default seed
#define LEHMER_SIZE 10 // arbitrary maximum of 10 seeds

int32_t lehmer_generate_modulo(int32_t z) {
    const uint32_t a = LEHMER_MULTIPLIER;
    const uint32_t m = LEHMER_MODULUS;

    int32_t r = (int32_t) (a * z) % m; // modulo
    return lehmer_seed_normalize_to_int(r, m);
}

int main(void) {
    int32_t initial_seed = LEHMER_SEED;
    int32_t current_seed = initial_seed;
    int32_t previous_seed = initial_seed;

    for (uint32_t i = 0; i < LEHMER_SIZE; i++) {}

    return 0;
}
