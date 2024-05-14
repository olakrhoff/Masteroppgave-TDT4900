import sys
import numpy as np
from sklearn.datasets import load_iris
from sklearn import tree
from matplotlib import pyplot as plt
import graphviz 
import math

def create_and_plot_tree(attribute_data_x, result_data_y, attribute_names=None, target_names=None, graph_name="decision_tree"):
    clf = tree.DecisionTreeClassifier(criterion="entropy", random_state=65, max_depth=len(attribute_names),  min_samples_leaf=math.ceil(len(result_data_y) * 0.05), min_impurity_decrease=0.1)
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
    result_data_y = []
    result_data_y_temp = []
    attribute_names = None
    target_names = None

    lines = 0
    try:
        with open(filepath, 'r') as file:
            for line in file:
                parsed = True
                if (lines == 0):
                    attribute_names = line.split(',')[1:7]
                    attribute_names.append("log_2(n^m)")
                else:
                    temp = line.split(',')
                    if ((lines - 1) % 403 == 0):
                        attribute_data_x.append([float(val) for val in temp[1:7]])
                        n = attribute_data_x[-1][0]
                        m = attribute_data_x[-1][1]
                        attribute_data_x[-1].append(m*np.log2(n))

                        if (len(result_data_y_temp) != 0):
                            result_data_y.append(result_data_y_temp)
                            result_data_y_temp = []
                    result_data_y_temp.append(float(temp[-1]))
                    if (target_names == None):
                        target_names = []
                    if (len(target_names) != 403):
                        target_names.append(''.join(temp[7:15]))
                lines += 1
        
        result_data_y.append(result_data_y_temp)
        


        # Now we need to find out which result did the best
        exclude_mip = True
        if (exclude_mip):
            for i, y_vals in enumerate(result_data_y):
                result_data_y[i][1] = 4000000000.0

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


