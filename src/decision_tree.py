import sys
import numpy as np
import pandas as pd
from sklearn.datasets import load_iris
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

def parse_file(filepath):
    parsed = False
    attribute_data_x = []
    result_data_y = {}
    attribute_names = None
    target_names = []
    opt_map = {}

    df = pd.read_csv(filepath)
    df.columns = df.columns.str.strip()

    df = df.sort_values(by=['Filename', 'UPPER_BOUND', 'B&B', 'NON_NAIVE', 'GOODS_ORDER', 'AGETNS_ORDER', 'REVERSE_GOODS_ORDER', 'REVERSE_AGENTS_ORDER', 'MIP_SOLVER'], ascending=[True, True, True, True, True, True, True, True, True])

    columns_to_join = ['UPPER_BOUND', 'B&B', 'NON_NAIVE', 'GOODS_ORDER', 'AGETNS_ORDER', 'REVERSE_GOODS_ORDER', 'REVERSE_AGENTS_ORDER', 'MIP_SOLVER']
    # Create a new column by joining the values of the specified columns with a space separator
    df['Optimisation'] = df[columns_to_join].astype(str).agg(' '.join, axis=1)
    df = df.drop(columns=columns_to_join)

    df['(m+1)log2(n+1)'] = (df['M'] + 1) * np.log2(df['N'] + 1)
    
    grouped = df.groupby('Filename').apply(lambda x: x[['TIME', 'Optimisation']].values.tolist())

    # Convert the grouped data into a list of lists
    list_of_time_arrays = grouped.tolist()

    print(len(list_of_time_arrays))

    filtered_list_of_time_arrays = [times for times in list_of_time_arrays if len(times) == 403]

    print(len(filtered_list_of_time_arrays))


    columns_to_check = ['Filename', 'N', 'M', 'M/N', 'AVG_PERM_DIST', 'AVG_VAL_DIST', 'BUDGET_PERCENT_USED']
    
    # Get the distinct combinations of these columns
    one_row_per_filename = df[columns_to_check].groupby('Filename').first().reset_index()

    one_row_per_filename = one_row_per_filename.sort_values(by='Filename')
    # Remove the 'Filename' column
    one_row_per_filename = one_row_per_filename.drop(columns=['Filename'])
    
    for column in one_row_per_filename.columns:
        one_row_per_filename[column] = pd.to_numeric(one_row_per_filename[column], errors='coerce')

    # Convert the result to a list of lists
    list_of_combinations = one_row_per_filename.values.tolist()
    
    # Print the list of distinct combinations
    print(list_of_combinations)

    #TODO: Get the target names and the result data

    return True, list_of_combinations, result_data_y, columns_to_check[1:], target_names

    grouped = df.groupby(['Filename', 'N', 'M', 'M/N', 'AVG_PERM_DIST', 'AVG_VAL_DIST', 'BUDGET_PERCENT_USED'])

    list_of_attribute = []
    for name, group in grouped:
        # Convert group data to list and append to the array
        list_of_attribute.append(group.values)    
    print(list_of_attribute)




    first_line = True
    try:
        with open(filepath, 'r') as file:
            for line in file:
                parsed = True
                if first_line:
                    attribute_names = line.split(',')[1:7]
                    attribute_names.append("(m+1)log_2(n+1)")
                    first_line = False
                else:
                    temp = line.split(',')

                    filename = temp[0]
                    if filename not in result_data_y:
                        result_data_y[filename] = {}
                    
                        attribute_data_x.append([float(val) for val in temp[1:7]])
                        n = attribute_data_x[-1][0]
                        m = attribute_data_x[-1][1]
                        attribute_data_x[-1].append((m+1)*np.log2(n + 1))

                    opt = ''.join(temp[7:15]).strip(" ");
                    if opt not in opt_map:
                        opt_map[opt] = len(opt_map)
                        target_names.append(opt)
                    
                    filename = temp[0]
                    if filename not in result_data_y:
                        result_data_y[filename] = {}

                    result_data_y[filename][opt] = float(temp[-1])
                    

        sorted_keys = sorted(opt_map.keys())

        # Create a new dictionary to hold the sorted keys and corresponding values
        sorted_opt_map = {}
        
        # Assign sorted keys and corresponding values to the new dictionary
        for i, key in enumerate(sorted_keys):
            sorted_opt_map[key] = i

        opt_map = sorted_opt_map

        exclude_mip = True
        opt_mip = '0 0 0 0 0 0 0 1'

        if (exclude_mip):
            for filename, y_vals in result_data_y.items():
                if opt_mip in y_vals:
                    y_vals[opt_mip] = 4000000000

        index_delete = []
        for i, vals in enumerate(result_data_y):
            max = True
            for v in vals:
                if (v != 4000000000):
                    max = False
                    break

            if (max):
                index_delete.append(i)

        index_delete.reverse()
        for index in index_delete:
            del result_data_y[index]
            del attribute_data_x[index]
    
        new_results = []
       
        for i, y_values in enumerate(result_data_y):
            # Find the index of the smallest element
            min_index = y_values.index(min(y_values))  # Get the index of the minimum value

            # Get the corresponding value from target_names
            new_results.append(min_index)
        result_data_y = new_results
        #result_data_y[0] = 1

        for i in range(0, 403):
            attribute_data_x.append([0, 0, 0, 0, 0, 0, 0])
            result_data_y.append(i)

        #result_data_y_flat = [item for sublist in result_data_y for item in sublist]
        #result_data_y_flat = [float(x) for x in result_data_y_flat]
        #print("Structure of result_data_y:", [len(x) for x in result_data_y])

    except FileNotFoundError:
        print("File (", filepath, ") could not be found")
        return False, None, None, None, None
    print(len(attribute_data_x))
    print(len(result_data_y))
    return parsed, attribute_data_x, result_data_y, attribute_names, target_names

if __name__ == "__main__":
    print("Creating decision tree...")

    filename = None
    if len(sys.argv) != 2:
        print("Incorrect numbers of arguments passed, expected one argument")
        print("python3 decision_tree.py <filepath>")
        exit(1)
    
    filename = sys.argv[-1]

    if (filename == "test"):
        # The test runs the test code with the iris dataset
        test()

    else:
        parsed, attribute_data_x, result_data_y, attribute_names, target_names = parse_file(filename)
        if (not parsed):
            print("Could not parse input file")
            exit(1)
        create_and_plot_tree(attribute_data_x, result_data_y, attribute_names, target_names)

    print("done")


