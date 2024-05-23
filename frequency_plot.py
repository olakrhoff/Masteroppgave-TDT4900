import csv
import argparse
import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
import os
import numpy as np

def parse_and_process_csv(file_path, columns_to_combine, new_column_name, separator=' '):
    try:
        # Read the CSV file into a pandas DataFrame, specifying dtype to avoid mixed types
        df = pd.read_csv(file_path, dtype=str)

        # Strip leading/trailing whitespace from column names
        df.columns = df.columns.str.strip()

        # Strip leading/trailing whitespace from each cell in the DataFrame
        df = df.map(lambda x: x.strip() if isinstance(x, str) else x)

        # Sort the DataFrame by "Filename" and then by "TIME"
        df = df.sort_values(by=["Filename", "TIME"])

        # Combine the specified columns into one new column
        df[new_column_name] = df[columns_to_combine].apply(lambda row: separator.join(row.values.astype(str)), axis=1)

        # Drop the original columns that were combined
        df = df.drop(columns=columns_to_combine)

        #df['RANK'] = df.groupby('Filename')['TIME'].rank(method='min').astype(int)

    except Exception as e:
        print("Error processing the CSV file:", e)
        return None
    
    return df

def create_frequency_array(df, new_column_name):
    # Get unique configurations and map them to indices
    unique_configs = df[new_column_name].unique()
    config_to_index = {config: idx for idx, config in enumerate(unique_configs)}

    # Initialize the frequency array
    num_configs = len(unique_configs)
    frequency_array = np.zeros((num_configs, num_configs), dtype=int)

    # Group by 'Filename' and populate the frequency array
    grouped = df.groupby('Filename')
    for filename, group in grouped:
        sorted_group = group.sort_values(by='TIME').reset_index(drop=True)
        for i, row in sorted_group.iterrows():
            config_index = config_to_index[row[new_column_name]]
            frequency_array[config_index][i] += 1

    # Debugging information to check the shape of the frequency array
    print(f"Frequency array shape: {frequency_array.shape}")
    print(f"Unique configs length: {len(unique_configs)}")

    return frequency_array, unique_configs

def plot_frequency_distributions(frequency_array, unique_configs, output_dir):
    try:
        # Ensure the output directory exists
        os.makedirs(output_dir, exist_ok=True)

        # Create a separate plot for each configuration
        for idx, config in enumerate(unique_configs):
            plt.figure(figsize=(12, 8))
            x_values = list(np.arange(1, 404))
            y_values = frequency_array[idx]
            print("kake")

            # Print statements for debugging
            print(f"Plotting configuration: {config}")
            print(f"x_values: {x_values[:10]}...{x_values[-10:]}")
            print(f"y_values: {y_values[:10]}...{y_values[-10:]}")

            sns.barplot(x=x_values, y=y_values)
            plt.title(f'Frequency of Rank Occurrences for Configuration: {config}')
            plt.xlabel('Rank')
            plt.ylabel('Frequency')
            plt.xticks(np.arange(1, 404, 20), rotation=45)
            plt.tight_layout()
            # Save the plot to a file
            plt.savefig(os.path.join(output_dir, f'{config}.png'))
            plt.close()
            print(f"Plot saved for configuration: {config}")

    except Exception as e:
        print("Error plotting the frequency plot:", e)


def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description='Parse and process a CSV file with specific data.')
    parser.add_argument('input_file', type=str, help='The path to the input CSV file to be parsed')
    parser.add_argument('output_file', type=str, help='The path to the output CSV file to be created')
    parser.add_argument('--separator', type=str, default=' ', help='Separator to use when combining columns')
    parser.add_argument('--output_dir', type=str, default='plots', help='Directory to save the plots')

    # Parse arguments
    args = parser.parse_args()
    
    # Define the columns to combine
    columns_to_combine = [
        'UPPER_BOUND', 'B&B', 'NON_NAIVE', 'GOODS_ORDER', 'AGETNS_ORDER',
        'REVERSE_GOODS_ORDER', 'REVERSE_AGENTS_ORDER', 'MIP_SOLVER'
    ]
    new_column_name = 'Combined_Column'

    # Parse and process the CSV file
    processed_data = parse_and_process_csv(args.input_file, columns_to_combine, new_column_name, args.separator)
    
    if processed_data is not None:
        # Write the processed data to the output CSV file
        processed_data.to_csv(args.output_file, index=False)
        print(f"Processed data has been written to {args.output_file}")

        frequency_array, unique_configs = create_frequency_array(processed_data, new_column_name)
        
        # Plot the rank frequency for each configuration
        plot_frequency_distributions(frequency_array, unique_configs, args.output_dir)

    
if __name__ == '__main__':
    main()

