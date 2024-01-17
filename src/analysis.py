import os
import matplotlib.pyplot as plt

save_path = 'plots/'

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

    x = []
    y = []
    a = []
    prev_len = 1
    with open(file_path, 'r') as file:
        for line in file:
            seg = line.split(': ')
            agents, goods, runs = seg[1].split(', ')
            if len(a) == 0 or agents != a[-1]:
                if len(a) > 0:
                    plt.plot(x, y, label='No. agents: ' + a[-1])
                    x = []
                    y = []
                    prev_len = len(a)
    
                a.append(agents)
            
            x.append(goods)
            
            measurments = [float(x) for x in seg[2].split(', ')]
            data = sum(measurments) / len(measurments)
            y.append(data)

    # Plot the last line we detected
    plt.plot(x, y, label='No. agents: ' + agents[-1])
    
    plt.xlabel('No. goods')
    plt.ylabel('Time (s)')
    plt.title(file_path.split('/')[-1])
    plt.legend()
    plt.savefig(save_path + file_path.split('/')[-1].split('.')[0] + ".png")


def start_analysis():
    print("Starting analysis...")
    data_folder_path = 'data'
    files = get_analysis_files(data_folder_path)

    for path in files:
        analyse(data_folder_path + '/' + path)
     
    print("Analysis done.")

if __name__ == "__main__":
    start_analysis()
