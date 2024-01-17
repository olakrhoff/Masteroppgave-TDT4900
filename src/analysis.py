import os
import matplotlib.pyplot as plt

save_path = 'plots/'
data_folder_path = 'data'

def get_analysis_files(folder_path):
    try:
        # Get the list of files in the folder
        files = os.listdir(folder_path)
        
        filtered_files = []

        # Filter out the files that are for analysis
        for file in files:
            if file.startswith("analysis"):
                filtered_files.append(file)
        
        files = filtered_files

        # Print the list of analysis files
        #print("Files in {}: {}".format(folder_path, files))
        
        return files
    except FileNotFoundError:
        print("The specified folder does not exist.")

def analyse(file_path):
    print("Analysing file: " + file_path)

    x_vals = [] # X-axis
    y_vals = [] # Y-axis
    nodes = []
    node = []
    x = []
    y = []
    a = [] # No. of agents for the lines
    prev_len = 1
    with open(file_path, 'r') as file:
        for line in file:
            seg = line.split(': ')
            agents, goods, _ = seg[1].split(', ')
            measurments = [float(x) for x in seg[2].split(', ')]
            num_nodes = measurments[-1]
            measurments = measurments[0:-1]
            if len(a) == 0 or agents != a[-1]:
                if len(a) > 0:
                    #plt.plot(x, y, label='No. agents: ' + a[-1])
                    x_vals.append(x)
                    y_vals.append(y)
                    nodes.append(node)
                    x = []
                    y = []
                    node = []
                    prev_len = len(a)
    
                a.append(agents)
            
            x.append(goods)
            data = sum(measurments) / len(measurments)
            y.append(data)
            node.append(num_nodes)

    # Plot the last line we detected
    #plt.plot(x, y, label='No. agents: ' + agents[-1])
    x_vals.append(x)
    y_vals.append(y)
    nodes.append(node)
    
    for i in range(len(x_vals)):
        plt.plot(x_vals[i], y_vals[i], label='No. agents: ' + a[i])

    plt.xlabel('No. goods')
    plt.ylabel('Time (s)')
    plt.title(file_path.split('/')[-1])
    plt.legend()
    plt.savefig(save_path + file_path.split('/')[-1].split('.')[0] + ".png")

    plt.close()


    for i in range(len(x_vals)):
        plt.plot(x_vals[i], nodes[i], label='No. agents: ' + a[i])


    plt.xlabel('No. goods')
    plt.ylabel('Nodes visited')
    plt.title(file_path.split('/')[-1])
    plt.legend()
    plt.savefig(save_path + "nodes_" + file_path.split('/')[-1].split('.')[0] + ".png")


def start_analysis():
    print("Starting analysis...")
    files = get_analysis_files(data_folder_path)

    for path in files:
        analyse(data_folder_path + '/' + path)
    
    print("Analysis done.")

if __name__ == "__main__":
    start_analysis()
