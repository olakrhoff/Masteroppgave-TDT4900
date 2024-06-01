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


        numeric_cols = df.columns.difference(['Filename'])
        df[numeric_cols] = df[numeric_cols].apply(pd.to_numeric, errors='coerce')

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

def create_frequency_array(df, opt_column):
    unique_configs = df[opt_column].unique()
    config_to_index = {config: idx for idx, config in enumerate(unique_configs)}

    # Initialize the frequency array
    num_configs = len(unique_configs)
    frequency_array = np.zeros((num_configs, num_configs), dtype=int)

    percentage_added_dict = {config: 0 for config in unique_configs}

    # Group by 'Filename' and populate the frequency array
    grouped = df.groupby('Filename')
    for filename, group in grouped:
        sorted_group = group.sort_values(by='TIME').reset_index(drop=True)
        for i, row in sorted_group.iterrows():
            if row['TIME'] == 4000000000:
                continue
            config_index = config_to_index[row[opt_column]]
            frequency_array[config_index][i] += 1

    for config in unique_configs:
        total_times = len(df[df[opt_column] == config])
        config_index = config_to_index[config]
        valid_times = sum(frequency_array[config_index])
        percentage_added_dict[config] = (valid_times / total_times) * 100 if total_times > 0 else 0


    # Debugging information to check the shape of the frequency array
    print(f"Frequency array shape: {frequency_array.shape}")
    print(f"Unique configs length: {len(unique_configs)}")

    return frequency_array, unique_configs, percentage_added_dict

def plot_frequency_distributions(frequency_array, unique_configs, output_dir, percentage_added_dict):
    try:
        # Ensure the output directory exists
        os.makedirs(output_dir, exist_ok=True)

        plt.rcParams.update({'font.size': 14, 'axes.titlesize': 18, 'axes.labelsize': 16, 'xtick.labelsize': 12, 'ytick.labelsize': 12, 'legend.fontsize': 14})

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

            data = pd.Series(y_values)
            window_size = 10
            smooth_data = data.rolling(window=window_size).mean()

            plt.plot(x_values, smooth_data, label=f'Percent not timed out: {percentage_added_dict[config]:.2f}%')
            plt.title(f'Frequency of Rank Occurrences for Configuration: {config}')
            plt.xlabel('Rank')
            plt.ylabel('Frequency')
            plt.xticks(np.arange(1, 404, 20), rotation=45)
            plt.legend()
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

        frequency_array, unique_configs, percent = create_frequency_array(processed_data, new_column_name)
        
        # Plot the rank frequency for each configuration
        plot_frequency_distributions(frequency_array, unique_configs, args.output_dir, percent)

    
if __name__ == '__main__':
    main()

