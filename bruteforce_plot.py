import matplotlib.pyplot as plt
import math
import random
import time

# ===============================
# Helper Functions
# ===============================
def distance(p1, p2):
    return math.hypot(p1[0] - p2[0], p1[1] - p2[1])

def path_length(points, path):
    return sum(distance(points[path[i]], points[path[(i + 1) % len(path)]])
               for i in range(len(path)))

def evaluate(points, indices, best):
    dist = path_length(points, indices)
    if dist < best['dist']:
        best['dist'] = dist
        best['path'] = indices.copy()

def tsp(points, indices, n, best, counter):
    counter[0] += 1
    if n == 1:
        evaluate(points, indices, best)
    else:
        for i in range(n):
            tsp(points, indices, n - 1, best, counter)
            swap = 0 if n % 2 == 0 else i
            indices[swap], indices[n - 1] = indices[n - 1], indices[swap]

# ===============================
# Measure Time and Recursion Count
# ===============================
def run_experiment(n):
    random.seed(42)
    points = [(random.uniform(0, 10), random.uniform(0, 10)) for _ in range(n)]
    indices = list(range(n))
    best = {'path': [], 'dist': float('inf')}
    counter = [0]
    
    start_time = time.time()
    tsp(points, indices, len(indices), best, counter)
    elapsed = time.time() - start_time
    
    return elapsed, counter[0], best

# ===============================
# Run for N = 3 to 10
# ===============================
n_values = list(range(3, 12))
times = []
recursions = []

for n in n_values:
    elapsed, count, _ = run_experiment(n)
    times.append(elapsed)
    recursions.append(count)
    print(f"N={n} | Time={elapsed:.4f}s | Recursions={count}")

# ===============================
# Plotting Results
# ===============================
plt.figure(figsize=(12,5))

# --- Time plot ---
plt.subplot(1, 2, 1)
plt.plot(n_values, times, 'o-b', lw=2)
plt.title("Time Taken vs Number of Points")
plt.xlabel("Number of Points (n)")
plt.ylabel("Time (seconds)")
plt.grid(True)

# --- Recursion plot ---
plt.subplot(1, 2, 2)
plt.plot(n_values, recursions, 'o-r', lw=2)
plt.title("Number of Recursions vs Number of Points")
plt.xlabel("Number of Points (n)")
plt.ylabel("Recursive Calls")
plt.grid(True)

plt.tight_layout()
plt.show()
