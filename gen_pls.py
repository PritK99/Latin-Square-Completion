import random
import os

def generate_complete_latin_square(n):
    square = [[(i + j) % n + 1 for j in range(n)] for i in range(n)]
    for i in range(n):
        random.shuffle(square[i])
    # Shuffle columns to make it more random
    cols = list(range(n))
    random.shuffle(cols)
    square = [[row[j] for j in cols] for row in square]
    return square

def generate_partial_from_complete(square, r):
    n = len(square)
    total = n * n
    num_to_keep = int(total * r)
    flattened_indices = list(range(total))
    random.shuffle(flattened_indices)
    keep_indices = set(flattened_indices[:num_to_keep])

    partial = [[0 for _ in range(n)] for _ in range(n)]
    for idx in keep_indices:
        i = idx // n
        j = idx % n
        partial[i][j] = square[i][j]

    return partial

def save_instance(n, r, idx, square, out_dir):
    filename = f"{out_dir}/LSC-{n}-{r:.1f}-{idx:03d}.txt"
    with open(filename, "w") as f:
        f.write(f"{n}\n")
        for row in square:
            f.write(" ".join(str(val) for val in row) + "\n")

def generate_all_instances():
    ns = [50, 60, 70]
    rs = [0.3, 0.4, 0.5, 0.6, 0.7, 0.8]
    num_instances = 100
    out_dir = "lsc_instances"
    os.makedirs(out_dir, exist_ok=True)

    for n in ns:
        for r in rs:
            for idx in range(1, num_instances + 1):
                complete = generate_complete_latin_square(n)
                partial = generate_partial_from_complete(complete, r)
                save_instance(n, r, idx, partial, out_dir)

generate_all_instances()
