import pandas as pd
import numpy as np
import sys

if len(sys.argv) < 2:
    print("Error: No input filename provided. Please provide an input CSV filename.")
    sys.exit(1)  # Exit with a non-zero status to indicate an error

input_filename = sys.argv[1]

# Check if the output filename is provided
output_filename = None
if len(sys.argv) >= 3:
    output_filename = sys.argv[2]

# Read the input CSV file
table = pd.read_csv(input_filename, sep=",")

table.columns = table.columns.str.strip()

# Get the name of the last column
last_column_name = table.columns[-1]

# Calculate the new column "log_2((N+1)^M)"
table["log_2((N+1)^M)"] = np.log2((table["N"].astype("uint64")) + 1) * (table["M"].astype("uint64") + 1) #* table["BUDGET_PERCENT_USED"].astype("float")

# Transform the last column using log base 2
table[last_column_name] = np.log2(table[last_column_name])

if 'Filename' in table.columns:
    table = table.drop(['Filename'], axis=1)

# Calculate the correlation matrix and round it to 2 decimal places
corr = round(table.corr(), 2)

if output_filename:
    # Write the correlation matrix to the output file
    with open(output_filename, 'w') as f:
        f.write(corr.to_string(float_format="%.2f"))
    print(f"Correlation matrix has been written to {output_filename}")
else:
    # Print the correlation matrix
    print(corr)

