import sys
from sklearn.datasets import load_iris
from sklearn import tree
from matplotlib import pyplot as plt
import graphviz 

def create_and_plot_tree(attribute_data_x, result_data_y, attribute_names=None, target_names=None, graph_name="decision_tree"):
    clf = tree.DecisionTreeClassifier()
    clf = clf.fit(attribute_data_x, result_data_y)
    
    tree.plot_tree(clf)
    
    dot_data = tree.export_graphviz(clf, out_file=None, 
                                    feature_names=attribute_names,  
                                    class_names=target_names,  
                                    filled=True, rounded=True,  
                                    special_characters=True)  
    graph = graphviz.Source(dot_data)  
    graph.render(graph_name)

def test():
    iris = load_iris()
    X, y = iris.data, iris.target
    create_and_plot_tree(X, y, iris.feature_names, iris.target_names, "test")

def parse_file(filepath):
    parsed = False
    attribute_data_x = []
    result_data_y = []
    attribute_names = None
    target_names = None

    lines = 0
    try:
        with open(filepath, 'r') as file:
            for line in file:
                parsed = True
                lines += 1
                print(line)
                if (lines == 1):
                    attribute_names = line.split(', ')
                elif (lines == 2):
                    target_names = line.split(', ')
                else:
                    temp = line.split(', ')
                    attribute_data_x.append([float(val) for val in temp[0:-1]])
                    result_data_y.append(float(temp[-1]))
    except FileNotFoundError:
        print("File (", filepath, ") could not be found")
        return False, None, None, None, None
    return parsed, attribute_data_x, result_data_y, attribute_names, None #target_names

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


