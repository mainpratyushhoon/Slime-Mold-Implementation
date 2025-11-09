import matplotlib.pyplot as plt
import math
import random
import time

N_POINTS = 11   
random.seed(42)

# Generate random 2D points
points = [(random.uniform(0, 10), random.uniform(0, 10)) for _ in range(N_POINTS)]

# ===============================
# Helper functions
# ===============================
def distance(i, j):
    x1, y1 = points[i]
    x2, y2 = points[j]
    return math.hypot(x2 - x1, y2 - y1)

def path_length(path):
    return sum(distance(path[i], path[(i + 1) % len(path)]) for i in range(len(path)))

# ===============================
# Recursive brute-force TSP
# ===============================
best_path = []
best_dist = float('inf')
recursion_count = 0

def tsp(indices, n):
    """Recursive Heap’s algorithm for generating all permutations."""
    global best_path, best_dist, recursion_count
    recursion_count += 1

    if n == 1:
        evaluate(indices)
    else:
        for i in range(n):
            tsp(indices, n - 1)
            swap = 0 if n % 2 == 0 else i
            indices[swap], indices[n - 1] = indices[n - 1], indices[swap]

def evaluate(indices):
    """Compute total distance of the current path."""
    global best_path, best_dist
    dist = path_length(indices)
    if dist < best_dist:
        best_dist = dist
        best_path = indices.copy()

# ===============================
# Run the algorithm
# ===============================
start_time = time.time()

indices = list(range(N_POINTS))
tsp(indices, len(indices))

end_time = time.time()
time_taken = end_time - start_time

print(f"Best path: {best_path}")
print(f"Shortest distance: {best_dist:.3f}")
print(f"Total recursive calls: {recursion_count}")
print(f"Time taken: {time_taken:.4f} seconds")

# ===============================
# Plotting
# ===============================
def plot_path(path):
    x = [points[i][0] for i in path + [path[0]]]
    y = [points[i][1] for i in path + [path[0]]]

    plt.figure(figsize=(6,6))
    plt.scatter(*zip(*points), color='red', s=80, label='Cities')
    plt.plot(x, y, 'b-', lw=2, label='Best Path')
    
    for i, (px, py) in enumerate(points):
        plt.text(px+0.1, py+0.1, str(i), fontsize=12, color='black')
    
    plt.title(f"Brute Force TSP | N={N_POINTS}\n"
              f"Distance = {best_dist:.2f} | Recursions = {recursion_count} | Time = {time_taken:.2f}s")
    plt.legend()
    plt.grid(True)
    plt.show()

plot_path(best_path)
