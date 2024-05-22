import argparse
import matplotlib.pyplot as plt

# Function to read numbers from a file
def read_numbers_from_file(filepath):
    with open(filepath, 'r') as file:
        numbers = [float(line.strip()) for line in file]
    return numbers

# Function to plot the numbers
def plot_numbers(numbers, x_label, y_label, title, sort_order):
    if sort_order == 'asc':
        numbers.sort()
    elif sort_order == 'desc':
        numbers.sort(reverse=True)
    plt.plot(numbers, marker='o')
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

    # Add mutually exclusive group for sorting
    sort_group = parser.add_mutually_exclusive_group()
    sort_group.add_argument('--sort-asc', action='store_const', const='asc', dest='sort_order', help='Sort the numbers in ascending order')
    sort_group.add_argument('--sort-desc', action='store_const', const='desc', dest='sort_order', help='Sort the numbers in descending order')

    # Parse the arguments
    args = parser.parse_args()

    # Read numbers from file
    numbers = read_numbers_from_file(args.filepath)
    
    # Plot the numbers
    plot_numbers(numbers, args.xlabel, args.ylabel, args.title, args.sort_order)

