# Script to plot the results
import matplotlib.pyplot as plt

# Example runtime data (replace these with your actual data)
dimensions = ["4x4", "5x5", "6x6", "7x7", "8x8", "9x9", "10x10"]
bestfs = [0.5788, 61.1732, 91.3431, 269.9811]  # only till 6x6
extrapolated_bestfs = [0.5788, 61.1732, 91.3431, 269.9811, 622.273]
plits_parr = [0.2357, 1.2872, 8.6459, 50.7565, 99.1762, 251.1907, 362.2763]
mis_parr = [0.0081, 0.0286, 0.0407, 0.2699, 3.936, 52.2047, 207.0122]  # proposed algorithm

# Plotting
plt.figure(figsize=(8, 6))

# Plot for Algorithm 1 (up to 6x6)
plt.plot(dimensions[:4], bestfs, label='BestFS', color='blue', marker='o')
plt.plot(dimensions[:5], extrapolated_bestfs, label='BestFS extrapolated', color='blue', linestyle='--')

# Plot for Algorithm 2 (all dimensions)
plt.plot(dimensions, plits_parr, label='PLITS Parallel', color='red', marker='s')

# Plot for Algorithm 3 (all dimensions, green for proposed algorithm)
plt.plot(dimensions, mis_parr, label='MIS Parallel', color='green', marker='^')

# Adding titles and labels
plt.title('Runtime Comparison of Algorithms')
plt.xlabel('Dimension (n x n)')
plt.ylabel('Runtime (seconds)')
plt.xticks(dimensions)  # ensures each dimension value appears on the x-axis
plt.legend()

# Display the plot
plt.grid(True)
plt.show()
