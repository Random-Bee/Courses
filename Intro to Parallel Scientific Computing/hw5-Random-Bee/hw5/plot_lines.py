import numpy as np
import matplotlib.pyplot as plt
import glob

# if plots directory does not exist, create it
import os
if not os.path.exists('plots'):
    os.makedirs('plots')

def linePlot(tid, procs):
    # procs -> [[2,2], [2,4], [4,4]]
    # get data for ser from T_x_y_{tid:06d}.dat
    # get data for par from T_x_y_{tid:06d}_*_{px}x{py}.dat
    # make one line plot with all data
    data = []
    
    # Read data for serial
    file_name = f'T_x_y_{tid:06d}.dat'
    
    print(f"Reading data from {file_name}")
    
    a = np.loadtxt(file_name)
    n = int(np.sqrt(a.shape[0]))
    
    x = a[::n, 0]  # Take every nth element from first column
    y = a[:n, 1]   # Take first n elements from second column
    T = a[:, 2].reshape((n, n))  # Reshape the third column to nxn
    
    mid_index = n // 2
    Tmid = T[:, mid_index]
    data.append((x, Tmid, 'Serial'))
    
    # Read data for parallel
    for px, py in procs:
        file_pattern = f'T_x_y_{tid:06d}_*_{px}x{py}.dat'
        file_list = sorted(glob.glob(file_pattern))
        if not file_list:
            raise FileNotFoundError(f"No files found for pattern: {file_pattern}")
        
        data_list = []
        for file_name in file_list:
            print(f"Reading data from {file_name}")
            a = np.loadtxt(file_name)
            data_list.append(a)
        
        a_combined = np.vstack(data_list)
        a_combined = a_combined[np.lexsort((a_combined[:, 1], a_combined[:, 0]))]
        
        n = int(np.sqrt(a_combined.shape[0]))
        x = a_combined[::n, 0]  # Take every nth element from first column
        y = a_combined[:n, 1]   # Take first n elements from second column
        T = a_combined[:, 2].reshape((n, n))  # Reshape the third column to nxn
        
        mid_index = n // 2
        Tmid = T[:, mid_index]
        data.append((x, Tmid, f'Parallel {px}x{py}'))
    
    # Plot line
    plt.figure()
    for x, Tmid, label in data:
        plt.plot(x, Tmid, '-', linewidth=2, label=label)
    plt.xlabel('x')
    plt.ylabel('T')
    plt.title(f'Profile along mid-y at t={tid:06d}')
    plt.xlim([-0.05, 1.05])
    plt.legend()
    plt.gca().tick_params(labelsize=14)
    line_filename = f'plots/line_midy_T_{tid:04d}_all.png'
    plt.savefig(line_filename, dpi=300)
    plt.close('all')
    
    print(f"Plot generated: {line_filename}")

linePlot(2553, [[2,2], [2,4], [4,4]])
linePlot(5106, [[2,2], [2,4], [4,4]])
linePlot(6385, [[2,2], [2,4], [4,4]])