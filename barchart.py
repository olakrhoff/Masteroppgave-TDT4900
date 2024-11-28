import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import sys

def plot_histogram(file_path):
    # Load the CSV file
    data = pd.read_csv(file_path, header=None, dtype=str)

    # Extract the values and convert to numeric, coercing errors to NaN and then dropping NaNs
    values = pd.to_numeric(data[0], errors='coerce').dropna()

    # Define the interval [0, pi/2]
    interval_start = 0
    interval_end = 1 #np.pi / 2

    # Define the number of bins (subsets) we want to divide the interval into
    num_bins = 8
    bins = np.linspace(interval_start, interval_end, num_bins + 1)

    # Create a histogram to count the number of values in each subset of the interval
    hist, bin_edges = np.histogram(values, bins=bins)

    # Plot the bar chart
    plt.figure(figsize=(10, 6))
    plt.bar(bin_edges[:-1], hist, width=(interval_end - interval_start) / num_bins, edgecolor='black')
    plt.xlabel('Average Permutation Distance')
    plt.ylabel('Frequency')
    plt.title('Frequency of Values in Average Permutation Distance Interval [0, 1]')
    plt.xticks(bin_edges, [f'{round(edge, 2)}' for edge in bin_edges])

    # Show the plot
    plt.show()

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python plot_histogram.py <path_to_csv_file>")
        sys.exit(1)

    file_path = sys.argv[1]
    plot_histogram(file_path)

