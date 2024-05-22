import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import argparse
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error

# Initialize the argument parser
parser = argparse.ArgumentParser(description='Save a scatter plot from a CSV file with headers for labels.')

# Add the CSV file path argument
parser.add_argument('csv_filepath', type=str, help='Path to the CSV file')

parser.add_argument('--reg', action='store_true', help='Include a regression line in the plots')
# Parse the arguments from the command line
args = parser.parse_args()

# Read the CSV file into a pandas DataFrame
data = pd.read_csv(args.csv_filepath)

data["log_2((n+1)^m)"] = np.log2(data['N'].astype("uint64") + 1) * data['M'].astype("uint64")
data["TIME"] = np.log2(data["TIME"])
# Get the list of column names
columns = data.columns

# The last column is the y-axis, but ignore the first column (datapoint name)
y_column = columns[-2]
y_column_label = "log_2(TIME)"#columns[-1]  # The last column


# Loop through all columns except the first and last
for idx, x_column in enumerate(columns):
    # Create a scatter plot
    plt.scatter(data[x_column], data[y_column])

    if args.reg:
        x_vals = data[x_column].values.reshape(-1, 1)  # Reshape for sklearn
        y_vals = data[y_column].values  # The y-axis data
        # Create and fit the linear regression model
        model = LinearRegression()
        model.fit(x_vals, y_vals)

        # Predict y-values based on the regression model
        y_pred = model.predict(x_vals)

        slope = model.coef_[0]
        intercept = model.intercept_

        mse = mean_squared_error(y_vals, y_pred)
        rmse = np.sqrt(mse)

        # Plot the linear regression line
        plt.plot(data[x_column], y_pred, color='red', linestyle='--', label=f'Regression (slope={slope:.2f}, intercept={intercept:.2f}, RMSE={rmse:.2f})')

    # Add labels and a title to the plot
    plt.xlabel(x_column)
    plt.ylabel(y_column_label)
    plt.title(f'Scatter Plot of {x_column} vs {y_column}')

    if args.reg:
        plt.legend()

    # Generate a unique output filename
    name = args.csv_filepath.split('.')[0]
    output_filename = f"{name}_{idx}"
    if args.reg:
        output_filename += "_reg"
    output_filename += ".png"
    # Save the plot to the specified file
    plt.savefig(output_filename)

    # Clear the current plot after saving
    plt.close()

    print(f"Plot saved to {output_filename}")
