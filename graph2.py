import matplotlib.pyplot as plt
import numpy as np

# Data
algorithms = ['BFS', 'DFS', 'BestFS', 'PLITS_seq', 'PLITS_par']
times_3x3 = [0.267743, 3.31479, 0.0033513, 0.99, 0.89]
times_4x4 = [0.5, 4.0, 0.004, 1.2, 1.1]  # Example data
times_5x5 = [0.8, 5.2, 0.005, 1.5, 1.4]  # Example data

# Create subplots
fig, axes = plt.subplots(1, 3, figsize=(15, 5))

# Plot for 3x3
axes[0].bar(algorithms, times_3x3, color='b')
axes[0].set_title('3x3 Configuration')
axes[0].set_ylabel('Time (s)')
axes[0].set_ylim(0, max(times_3x3) * 1.2)

# Plot for 4x4
axes[1].bar(algorithms, times_4x4, color='g')
axes[1].set_title('4x4 Configuration')
axes[1].set_ylim(0, max(times_4x4) * 1.2)

# Plot for 5x5
axes[2].bar(algorithms, times_5x5, color='r')
axes[2].set_title('5x5 Configuration')
axes[2].set_ylim(0, max(times_5x5) * 1.2)

# Adjust layout and show plot
plt.tight_layout()
plt.show()
