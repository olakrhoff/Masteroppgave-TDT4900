import pandas as pd
import matplotlib.pyplot as plt
import argparse
import seaborn as sns
import statsmodels.api as sm
from matplotlib.lines import Line2D

def plot_data(file_path, plot_title, x_label):
    # Load the data into a pandas DataFrame
    data = pd.read_csv(file_path)

    # Set the 'log' column as the x-axis
    x_column = data.columns[1]

    # Ensure 'log' is in the dataframe
    if x_column not in data.columns:
        raise ValueError(f"'{x_column}' column is not found in the data")

    markers = ['o', 's', 'D', '^', 'v', '<', '>', 'p', '*', 'H', 'X']
    line_styles = ['-', '--', '-.', ':', '-', '--', '-.', ':', '-', '--']
    colours = sns.color_palette("Set1")
    marker_index = 0
    line_style_index = 0
    colour_index = 0
    
    legend_handles = []
    # Create a plot
    plt.figure(figsize=(12, 8))

    # Iterate over each column and plot it against 'log' using regplot
    for column in data.columns:
        if column != x_column and column != 'Filename':
            sns.regplot(order=1, scatter=True, logx=False, label=column, x=x_column, y=column, data=data, ci=95, marker=markers[marker_index], line_kws={'linestyle': line_styles[line_style_index]}, color=colours[colour_index])

            handle = Line2D(
                [0], [0],
                marker=markers[marker_index],
                linestyle=line_styles[line_style_index],
                color=colours[colour_index],  # Assuming a single color for simplicity
                label=column
            )
            legend_handles.append(handle)

            marker_index += 1
            marker_index %= len(markers)
            line_style_index += 1
            line_style_index %= len(line_styles)
            colour_index += 1
            colour_index %= len(colours)
            #X = sm.add_constant(data[x_column])  # Adds a constant term to the predictor
            #print(X)
            #y = data[column]
            #model = sm.OLS(y, X).fit()
            #coefficients = model.params

            #print(f"Regression coefficients for {column}:")
            #print(f"Intercept: {coefficients[0]}, Slope: {coefficients[1]}")
    # Add labels and title
    if x_label == '':
        plt.xlabel('# of Nodes: log2((n+1)^(m+1) - 1)')
    else:
        plt.xlabel(x_label)

    plt.ylabel('Time: log2(Time) of microseconds')

    if plot_title:
        plt.title(plot_title)
    else:
        plt.title('ADD TITLE')

    plt.legend(handles=legend_handles, loc='upper left', ncol=1)

    # Show the plot
    plt.show()

if __name__ == "__main__":
    # Set up argument parser
    parser = argparse.ArgumentParser(description='Plot data from a CSV file.')
    parser.add_argument('filename', type=str, help='The path to the CSV file to be plotted.')
    parser.add_argument('--title', type=str, default='', help='The title for the plot.')
    parser.add_argument('--x', type=str, default='', help='The label for the x-axis.')

    # Parse arguments
    args = parser.parse_args()

    # Call the plot function with the provided filename
    plot_data(args.filename, args.title, args.x)

