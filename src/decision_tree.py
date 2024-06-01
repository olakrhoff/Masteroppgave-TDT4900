import sys
import argparse
import numpy as np
import pandas as pd
from sklearn.datasets import load_iris
from sklearn.metrics import accuracy_score
from sklearn import tree
from matplotlib import pyplot as plt
import graphviz 
import math

def create_and_plot_tree(attribute_data_x, result_data_y, attribute_names=None, target_names=None, graph_name="decision_tree"):
    clf = tree.DecisionTreeClassifier(criterion="entropy", splitter="best", random_state=65, max_depth=len(attribute_names),  min_samples_leaf=math.ceil(len(result_data_y) * 0.05), min_impurity_decrease=0.1)
    clf = clf.fit(attribute_data_x, result_data_y)
   
    #target_names[0] = "naive"
    #target_names[1] = "mip"
    tree.plot_tree(clf)
    dot_data = tree.export_graphviz(clf, out_file=None, 
                                    feature_names=attribute_names,  
                                    class_names=target_names,  
                                    filled=True, rounded=True,  
                                    special_characters=True,
                                    proportion=False)  
    graph = graphviz.Source(dot_data)  
    graph.render(graph_name)

    # Make predictions on the entire dataset
    y_pred = clf.predict(attribute_data_x)
    
    # Calculate and print the accuracy
    accuracy = accuracy_score(result_data_y, y_pred)
    print(f"Classification accuracy: {accuracy * 100:.2f}%")
    
    # Count correct classifications
    correct_classifications = np.sum(result_data_y == y_pred)
    print(f"Number of correct classifications: {correct_classifications} out of {len(result_data_y)}")
    

    #result_data_y = np.array(result_data_y)  # Convert to numpy array for consistency
    #best_y = np.array([min(arr) for arr in result_data_y])
    
    #predict = clf.predict(np.array(attribute_data_x))
    #print(predict)
def test():
    iris = load_iris()
    X, y = iris.data, iris.target
    create_and_plot_tree(X, y, iris.feature_names, iris.target_names, "test")
    print(X)
    print(y)
    print(iris.feature_names)
    print(iris.target_names)

def parse_file(filepath, ignore_MIP):
    df = pd.read_csv(filepath)
    df.columns = df.columns.str.strip()

    df = df.sort_values(by=['Filename', 'UPPER_BOUND', 'B&B', 'NON_NAIVE', 'GOODS_ORDER', 'AGETNS_ORDER', 'REVERSE_GOODS_ORDER', 'REVERSE_AGENTS_ORDER', 'MIP_SOLVER'], ascending=[True, True, True, True, True, True, True, True, True])

    columns_to_join = ['UPPER_BOUND', 'B&B', 'NON_NAIVE', 'GOODS_ORDER', 'AGETNS_ORDER', 'REVERSE_GOODS_ORDER', 'REVERSE_AGENTS_ORDER', 'MIP_SOLVER']
    # Create a new column by joining the values of the specified columns with a space separator
    df['Optimisation'] = df[columns_to_join].astype(str).agg(' '.join, axis=1)
    df = df.drop(columns=columns_to_join)

    unique_optimizations = df['Optimisation'].unique()
    opt_map = {opt: idx for idx, opt in enumerate(unique_optimizations)}
    df['OptIndex'] = df['Optimisation'].map(opt_map)


    df['(m+1)log2(n+1)'] = (df['M'] + 1) * np.log2(df['N'] + 1)
    
    columns_to_check = ['Filename', 'N', 'M', 'M/N', 'AVG_PERM_DIST', 'AVG_VAL_DIST', 'BUDGET_PERCENT_USED']
    one_row_per_filename = df[columns_to_check].groupby('Filename').first().reset_index()
    one_row_per_filename = one_row_per_filename.sort_values(by='Filename')

    best_runs = []

    grouped = df.groupby('Filename')
    index_MIP = 1
    for filename, group in grouped:
        if ignore_MIP:
            group = group[group['OptIndex'] != index_MIP]
        min_time_idx = group['TIME'].idxmin()
        best_run_opt_index = group.loc[min_time_idx, 'OptIndex']
        best_time = group.loc[min_time_idx, 'TIME']

        if best_time == 4000000000:
            one_row_per_filename = one_row_per_filename[one_row_per_filename['Filename'] != filename]
        else:
            best_runs.append(best_run_opt_index)

    
    # Get the distinct combinations of these columns
    # Remove the 'Filename' column
    one_row_per_filename = one_row_per_filename.drop(columns=['Filename'])
    
    for column in one_row_per_filename.columns:
        one_row_per_filename[column] = pd.to_numeric(one_row_per_filename[column], errors='coerce')

    # Convert the result to a list of lists
    list_of_combinations = one_row_per_filename.values.tolist()
    
    # Print the list of distinct combinations
    #print(list_of_combinations)

    for opt_name, opt_index in opt_map.items():
        list_of_combinations.append([0, 0, 0, 0, 0, 0])
        best_runs.append(opt_index)

    return True, list_of_combinations, best_runs, columns_to_check[1:], unique_optimizations

if __name__ == "__main__":
    print("Creating decision tree...")
    
    parser = argparse.ArgumentParser(description="Create decision tree from input data")
    parser.add_argument("filepath", metavar="filepath", type=str, help="Path to input file")
    parser.add_argument("--ignoreMIP", action="store_true", help="Ignore MIP optimization")

    # Parse command-line arguments
    args = parser.parse_args()

    filename = args.filepath
    
    if (filename == "test"):
        # The test runs the test code with the iris dataset
        test()
    else:
        parsed, attribute_data_x, result_data_y, attribute_names, target_names = parse_file(filename, args.ignoreMIP)
        if (not parsed):
            print("Could not parse input file")
            exit(1)
        create_and_plot_tree(attribute_data_x, result_data_y, attribute_names, target_names)

    print("done")


