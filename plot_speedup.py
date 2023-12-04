import matplotlib.pyplot as plt

# Read speedup results from the file
with open("speedup_results.txt", "r") as file:
    lines = file.readlines()
    data = [list(map(float, line.split())) for line in lines]

# Extract threads and speedup values
threads, speedup = zip(*data)

# Plot the speedup graph
plt.plot(threads, speedup, marker='o')
plt.title("Speedup vs. Number of Threads")
plt.xlabel("Number of Threads")
plt.ylabel("Speedup")
plt.grid(True)
plt.show()
