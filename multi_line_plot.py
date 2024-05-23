import pandas as pd
import matplotlib.pyplot as plt
import argparse
import seaborn as sns
import statsmodels.api as sm

def plot_data(file_path):
    # Load the data into a pandas DataFrame
    data = pd.read_csv(file_path)

    # Set the 'log' column as the x-axis
    x_column = data.columns[1]

    # Ensure 'log' is in the dataframe
    if x_column not in data.columns:
        raise ValueError(f"'{x_column}' column is not found in the data")

    # Create a plot
    plt.figure(figsize=(12, 8))

    # Iterate over each column and plot it against 'log' using regplot
    for column in data.columns:
        if column != x_column and column != 'Filename':
            sns.regplot(scatter=True, logx=False, label=column, x=x_column, y=column, data=data, ci=95)

            X = sm.add_constant(data[x_column])  # Adds a constant term to the predictor
            print(X)
            y = data[column]
            model = sm.OLS(y, X).fit()
            coefficients = model.params

            print(f"Regression coefficients for {column}:")
            print(f"Intercept: {coefficients[0]}, Slope: {coefficients[1]}")
    # Add labels and title
    plt.xlabel('# of Nodes: log2((n+1)^m)')
    plt.ylabel('Time: log2(Time) of microseconds')
    plt.title('Regression Plots with log as x-axis')
    plt.legend(loc='upper left')

    # Show the plot
    plt.show()

if __name__ == "__main__":
    # Set up argument parser
    parser = argparse.ArgumentParser(description='Plot data from a CSV file.')
    parser.add_argument('filename', type=str, help='The path to the CSV file to be plotted.')

    # Parse arguments
    args = parser.parse_args()

    # Call the plot function with the provided filename
    plot_data(args.filename)

