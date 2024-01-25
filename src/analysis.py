import os
import matplotlib.pyplot as plt
import numpy as np

save_path = 'plots/'
data_folder_path = 'data'


def exponential_fitting(x_vals, y_vals):
    rows = len(x_vals)
    print(x_vals)
    cols = 2
    
    # The model we are fitting is: y = c1*e^(m*c2)
    # We linearise it to: ln(y) = ln(c1) + m*(c2*1)
    #                     ln(y) = k1     + m*k2
    # Linear model:       y     = a      + x*b

    # Model the data x-axis observations
    A = np.ones((rows, cols))
    A[:, 1] = [x for x in x_vals]
    
    # Model the data y-axis measurments
    b = np.ones((rows, 1))
    b[:, 0] = [np.log(y) for y in y_vals]

    # We do a QR-factorisation of A to have a better conditioning number than
    # if we used ATA = A.T @ A
    # This gives us the system: Ax = b
    #                           QRx= b
    #                           Rx = Q.Tb
    #                           Rx = d
    # Q is an orthogonal matrix, which gives us: Q^-1 = Q.T
    Q, R = np.linalg.qr(A)
    d = Q.T @ b
    k1, k2 = np.linalg.solve(R, d)
    # k1 = ln(c1)
    c1 = np.exp(k1)[0] 
    # k2 = c2
    c2 = k2[0]

    x = np.linspace(x_vals[0], x_vals[-1], 1000)
    y = [c1*(np.exp(val * c2)) for val in x]
    cond = np.linalg.cond(R)
    
    residuals = []
    for i in range(len(x_vals)):
        r = y_vals[i] - (c1*(np.exp(x_vals[i] * c2)))
        residuals.append(r)
    print(residuals)
    residuals2 = [r**2 for r in residuals]
    SE = sum(residuals2)
    m = rows
    RMSE = np.sqrt(SE / m)

    return x, y, cond, RMSE


def fit_data_to_model(x_vals, y_vals, model_type):
    if model_type == "LIN":
        print("TODO: Implement linear model fitting")
    elif model_type == "EXP":
        return exponential_fitting(x_vals, y_vals)
    else:
        print("Did not recoginise the model_type: " + model_type)
        exit(1)

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
    a = [] # No. of agents for the lines
    with open(file_path, 'r') as file:
        for line in file:
            seg = line.split(': ')
            agents, goods, _, num_nodes = seg[1].split(', ')
            measurments = [float(x) for x in seg[2].split(', ')]
            
            for m in measurments: 
                x_vals.append(int(goods))
                y_vals.append(m)
                nodes.append(num_nodes)
                a.append(agents)

    plt.scatter(x_vals, [val / 1000000 for val in y_vals], label='No. agents: ' + a[0])

    model_x, model_y, condition_number, RMSE = fit_data_to_model(x_vals, y_vals, "EXP")

    plt.plot(model_x, [val / 1000000 for val in model_y],
             label='Exponential model, cond: ' +
             "{:.2e}".format(condition_number) + 
             ", RMSE: " + "{:.2e}".format(RMSE))

    plt.xlabel('No. goods')
    plt.ylabel('Time (s)')
    plt.title(file_path.split('/')[-1])
    plt.legend()
    plt.savefig(save_path + file_path.split('/')[-1].split('.')[0] + ".png")

    plt.close()
    plt.clf()

def start_analysis():
    print("Starting analysis...")
    files = get_analysis_files(data_folder_path)

    for path in files:
        analyse(data_folder_path + '/' + path)
    
    print("Analysis done.")

if __name__ == "__main__":
    start_analysis()
