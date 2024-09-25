"""
Script: lehmer.gamma
"""

from concurrent.futures import ThreadPoolExecutor

seed = 1337
MODULUS = 2147483647
MULTIPLIER = 48271
BATCH_SIZE = 10000  # Number of seeds to generate per batch


# g(z) = a * (z mod q) - r * (z // q)
def gamma(z: int, a: int, m: int) -> int:
    q = m // a
    r = m % a
    return (a * (z % q) - r * (z // q)) % m


# Generate a batch of seeds
def generate_batch(seed: int, batch_size: int) -> list:
    z = seed
    batch = []
    for _ in range(batch_size):
        z = gamma(z, MULTIPLIER, MODULUS)
        batch.append(z)
    return batch


# Batch processing function
def process_batches(initial_seed, num_batches, batch_size):
    seeds = []
    z = initial_seed
    for _ in range(num_batches):
        batch = generate_batch(z, batch_size)
        seeds.extend(batch)
        z = batch[-1]  # Use last seed of the batch as input for the next batch
    return seeds


# Threaded version
def threaded_batch_processing(seed, total_seeds, batch_size):
    num_batches = total_seeds // batch_size
    with ThreadPoolExecutor() as executor:
        results = list(
            executor.map(
                generate_batch,
                [seed] * num_batches,
                [batch_size] * num_batches,
            )
        )

    # Flatten batches
    seeds = [item for sublist in results for item in sublist]
    return seeds


if __name__ == "__main__":
    # Example: Generate seeds in batches
    total_seeds = 100_000_000
    seeds = threaded_batch_processing(seed, total_seeds, BATCH_SIZE)
    print(f"Generated {len(seeds)} seeds.")
