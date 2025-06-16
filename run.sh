#!/bin/bash
clang++ -fopenmp -lomp -O3 -march=native -std=c++17 ./src/mis_par.cpp -o ./lsc_solver

python3 gen_pls.py

BINARY=./lsc_solver
INSTANCE_DIR=./lsc_instances
OUTPUT_DIR=./results
mkdir -p "$OUTPUT_DIR"

# Initialize CSV file
echo "Instance,ElapsedTime(s)" > "$OUTPUT_DIR/times.csv"

for file in "$INSTANCE_DIR"/*.txt; do
    echo "Processing $file..."
    filename=$(basename "$file")
    "$BINARY" "$file" > "$OUTPUT_DIR/${filename%.txt}.log" 2>&1
    echo "Done with $filename"
    sleep 10
done

echo "All files processed!"
