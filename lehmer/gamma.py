"""
Script: lehmer.gamma
"""

seed = 1337
MODULUS = 2147483647
MULTIPLIER = 48271


# \gamma(z) = a \cdot (z \mod q) - r \cdot (z \div q)
def gamma(z: int, a: int, m: int) -> int:
    q = int(m / a)
    r = int(m % a)
    return int((a * (z % q)) - (r * (z / q)))


z = seed
errors = 0
for i in range(MODULUS):
    # generate next seed
    z = gamma(z, MULTIPLIER, MODULUS)
    # bind generated seed
    z = int(z) % int(MODULUS)
    # normalize generated seed
    norm = float(z) / float(MODULUS)
    # count errors
    if not (0 < norm < 1):
        # print("bind =", bind, "norm =", norm)
        errors += 1

print("violated boundary", errors, " times.")
