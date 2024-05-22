import argparse
import matplotlib.pyplot as plt
from collections import Counter

# Function to read numbers from a file
def read_numbers_from_file(filepath):
    with open(filepath, 'r') as file:
        numbers = [float(line.strip()) for line in file]
    return numbers

# Function to plot the numbers
def plot_numbers(numbers, x_label, y_label, title):
    occurrences = Counter(numbers)

    # Convert to a dictionary if needed (optional)
    occurrences_dict = dict(sorted(occurrences.items()))


    plt.plot(occurrences_dict.keys(), occurrences_dict.values(), marker='o')
    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.grid(True)
    plt.show()

if __name__ == "__main__":
    # Initialize the argument parser
    parser = argparse.ArgumentParser(description='Plot numbers from a file.')

    # Add arguments
    parser.add_argument('filepath', type=str, help='Path to the file containing numbers')
    parser.add_argument('--xlabel', type=str, default='Index', help='Label for the x-axis')
    parser.add_argument('--ylabel', type=str, default='Value', help='Label for the y-axis')
    parser.add_argument('--title', type=str, default='Numbers Plot', help='Title of the plot')

    # Parse the arguments
    args = parser.parse_args()

    # Read numbers from file
    numbers = read_numbers_from_file(args.filepath)
    
    # Plot the numbers
    plot_numbers(numbers, args.xlabel, args.ylabel, args.title)

