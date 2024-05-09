import pandas as pd
import numpy as np
import sys

if len(sys.argv) < 2:
    print("Error: No filename provided. Please provide a CSV filename.")
    sys.exit(1)  # Exit with a non-zero status to indicate an error

filename = sys.argv[1]

table = pd.read_csv(filename, sep=",")

last_column_name = table.columns[-1]

table["log_2(N^M)"] = np.log2(table["N"].astype("uint64")) * table["M"].astype("uint64")

table[last_column_name] = np.log10(table[last_column_name])

corr = round(table.corr(), 2)

print(corr)
