"""
Script: lehmer.gamma
"""

seed = 1337
MODULUS = 2147483647
MULTIPLIER = 48271
ITERATIONS = 1000000  # For performance testing


# bind the output to the range [0, 1]
def normalize_float(z: int) -> float:
    return float(z) / float(MODULUS)


# g(z) = a * (z mod q) - r * (z // q)
def gamma(z: int, a: int, m: int) -> int:
    q = m // a
    r = m % a
    result = a * (z % q) - r * (z // q)
    return result % m  # bind output to [0, m - 1]


z = seed
errors = 0
for i in range(ITERATIONS):
    # generate next seed
    z = gamma(z, MULTIPLIER, MODULUS)
    # normalize generated seed
    norm = normalize_float(z)
    # count errors
    if not (0 < norm < 1):
        print("seed =", z, "norm =", norm)
        errors += 1

print("violated boundary", errors, "times.")
