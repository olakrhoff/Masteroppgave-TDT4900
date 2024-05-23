import csv
import argparse
import pandas as pd

def parse_and_process_csv(file_path):
    try:
        # Read the CSV file into a pandas DataFrame, specifying dtype to avoid mixed types
        df = pd.read_csv(file_path, dtype=str)


        df.columns = df.columns.str.strip()

        # Strip leading/trailing whitespace from each cell in the DataFrame
        df = df.map(lambda x: x.strip() if isinstance(x, str) else x)


        # Print DataFrame columns to debug the issue
        print("DataFrame columns:", df.columns)

        # Convert GOODS_ORDER and AGETNS_ORDER into dummy variables if they exist in the DataFrame
        if 'GOODS_ORDER' in df.columns and 'AGETNS_ORDER' in df.columns:
            goods_order_dummies = pd.get_dummies(df['GOODS_ORDER'], prefix='GOODS_ORDER')
            agetns_order_dummies = pd.get_dummies(df['AGETNS_ORDER'], prefix='AGETNS_ORDER')

            # Convert boolean dummies to integers (0 or 1)
            goods_order_dummies = goods_order_dummies.astype(int)
            agetns_order_dummies = agetns_order_dummies.astype(int)

            # Drop the original columns and concatenate the dummy variables
            df = df.drop(['GOODS_ORDER', 'AGETNS_ORDER'], axis=1)
            df = pd.concat([df, goods_order_dummies, agetns_order_dummies], axis=1)
        else:
            print("Missing expected columns: 'GOODS_ORDER' or 'AGETNS_ORDER'")
        
        df = df.drop(['Filename'], axis=1)
    
    except Exception as e:
        print("Error processing the CSV file:", e)
        return None
    
    return df

def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description='Parse and process a CSV file with specific data.')
    parser.add_argument('input_file', type=str, help='The path to the input CSV file to be parsed')
    parser.add_argument('output_file', type=str, help='The path to the output CSV file to be created')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Parse and process the CSV file
    processed_data = parse_and_process_csv(args.input_file)
    
    if processed_data is not None:
        # Write the processed data to the output CSV file
        processed_data.to_csv(args.output_file, index=False)
        print(f"Processed data has been written to {args.output_file}")

if __name__ == '__main__':
    main()

