import random
import os

def generate_cyclic_latin_square(n):
    """Generate a basic cyclic Latin square"""
    square = [[(i + j) % n + 1 for j in range(n)] for i in range(n)]
    return square

def apply_valid_transformations(square):
    """Apply transformations that preserve the Latin square property"""
    n = len(square)

    # Method 1: Row permutation only (always safe)
    if random.choice([True, False]):
        row_indices = list(range(n))
        random.shuffle(row_indices)
        square = [square[row_indices[i]] for i in range(n)]

    # Method 2: Column permutation only (always safe)
    if random.choice([True, False]):
        col_indices = list(range(n))
        random.shuffle(col_indices)
        square = [[square[i][col_indices[j]] for j in range(n)] for i in range(n)]

    # Method 3: Symbol permutation (always safe)
    if random.choice([True, False]):
        symbols = list(range(1, n + 1))
        random.shuffle(symbols)
        symbol_map = {i + 1: symbols[i] for i in range(n)}
        square = [[symbol_map[square[i][j]] for j in range(n)] for i in range(n)]

    return square

def verify_latin_square(square):
    """Verify that a square is a valid Latin square"""
    n = len(square)

    # Check that each row contains each number exactly once
    for i in range(n):
        row_set = set(square[i])
        if len(row_set) != n or row_set != set(range(1, n + 1)):
            return False

    # Check that each column contains each number exactly once
    for j in range(n):
        col_set = set(square[i][j] for i in range(n))
        if len(col_set) != n or col_set != set(range(1, n + 1)):
            return False

    return True

def generate_partial_from_complete(square, r):
    """Generate a partial Latin square by removing cells"""
    n = len(square)
    total = n * n
    num_to_keep = int(total * r)

    # Create list of all positions
    positions = [(i, j) for i in range(n) for j in range(n)]
    random.shuffle(positions)

    # Keep only the first num_to_keep positions
    keep_positions = set(positions[:num_to_keep])

    # Create partial square
    partial = [[0] * n for _ in range(n)]
    for i, j in keep_positions:
        partial[i][j] = square[i][j]

    return partial

def check_partial_feasibility(partial):
    """Basic feasibility check for partial Latin square"""
    n = len(partial)

    # Check rows for duplicates
    for i in range(n):
        row_values = [partial[i][j] for j in range(n) if partial[i][j] != 0]
        if len(row_values) != len(set(row_values)):
            return False

    # Check columns for duplicates
    for j in range(n):
        col_values = [partial[i][j] for i in range(n) if partial[i][j] != 0]
        if len(col_values) != len(set(col_values)):
            return False

    return True

def save_instance(n, r, idx, square, out_dir):
    """Save instance to file"""
    filename = f"{out_dir}/LSC-{n}-{r:.1f}-{idx:03d}.txt"
    with open(filename, "w") as f:
        f.write(f"{n}\n")
        for row in square:
            f.write(" ".join(str(val) for val in row) + "\n")

def generate_all_instances():
    """Generate all PLS instances"""
    ns = [20, 50, 60, 70]
    rs = [0.3, 0.4, 0.5, 0.6, 0.7, 0.8]
    num_instances = 100
    out_dir = "lsc_instances"
    os.makedirs(out_dir, exist_ok=True)

    failed_generations = 0

    for n in ns:
        print(f"Generating instances for n={n}...")
        for r in rs:
            print(f"  Fill ratio r={r}...")
            for idx in range(1, num_instances + 1):
                max_attempts = 10
                success = False

                for attempt in range(max_attempts):
                    try:
                        # Generate complete Latin square
                        complete = generate_cyclic_latin_square(n)

                        # Apply valid transformations
                        complete = apply_valid_transformations(complete)

                        # Verify it's still a valid Latin square
                        if not verify_latin_square(complete):
                            continue

                        # Generate partial square
                        partial = generate_partial_from_complete(complete, r)

                        # Basic feasibility check
                        if not check_partial_feasibility(partial):
                            continue

                        # Save the instance
                        save_instance(n, r, idx, partial, out_dir)
                        success = True
                        break

                    except Exception as e:
                        print(f"    Error generating instance {idx}: {e}")
                        continue

                if not success:
                    failed_generations += 1
                    print(f"    Failed to generate instance {idx} after {max_attempts} attempts")

                # Progress indicator
                if idx % 10 == 0:
                    print(f"    Generated {idx}/{num_instances} instances for n={n}, r={r}")

    print(f"\nGeneration complete. Failed generations: {failed_generations}")

def test_small_example():
    """Test the generation process with a small example"""
    print("Testing with small example...")
    n = 4
    complete = generate_cyclic_latin_square(n)
    print("Original cyclic square:")
    for row in complete:
        print(row)

    print("\nVerification:", verify_latin_square(complete))

    transformed = apply_valid_transformations(complete)
    print("\nAfter transformations:")
    for row in transformed:
        print(row)

    print("Verification after transformation:", verify_latin_square(transformed))

    partial = generate_partial_from_complete(transformed, 0.5)
    print("\nPartial square (50% filled):")
    for row in partial:
        print(row)

    print("Feasibility check:", check_partial_feasibility(partial))

if __name__ == "__main__":
    # Uncomment to test with small example first
    # test_small_example()

    # # Generate all instances
    generate_all_instances()
