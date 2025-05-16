import numpy as np
import matplotlib.pyplot as plt
import glob

# if plots directory does not exist, create it
import os
if not os.path.exists('plots'):
    os.makedirs('plots')

def contourPar(tid, px, py):
    file_pattern = f'T_x_y_{tid:06d}_*_{px}x{py}.dat'

    # Get list of files matching the pattern
    file_list = sorted(glob.glob(file_pattern))
    if not file_list:
        raise FileNotFoundError(f"No files found for pattern: {file_pattern}")

    # Initialize list to store data from all files
    data_list = []

    # Read and combine data from all files
    for file_name in file_list:
        print(f"Reading data from {file_name}")
        a = np.loadtxt(file_name)
        data_list.append(a)

    # Concatenate data along rows
    a_combined = np.vstack(data_list)

    # Sort data based on x and y
    a_combined = a_combined[np.lexsort((a_combined[:, 1], a_combined[:, 0]))]

    # Reshape combined data
    n = int(np.sqrt(a_combined.shape[0]))
    x = a_combined[::n, 0]  # Take every nth element from first column
    y = a_combined[:n, 1]   # Take first n elements from second column
    T = a_combined[:, 2].reshape((n, n))  # Reshape the third column to nxn

    # Plot contour
    plt.figure()
    plt.contourf(x, y, T.T, cmap='jet')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title(f't = {tid:06d}, px = {px}, py = {py}')
    plt.xlim([-0.05, 1.05])
    plt.ylim([-0.05, 1.05])
    plt.clim([-0.05, 1.05])
    plt.colorbar()
    plt.gca().tick_params(labelsize=14)
    contour_filename = f'plots/cont_T_{tid:04d}_{px}x{py}.png'
    plt.savefig(contour_filename, dpi=300)

    plt.close('all')

    print(f"Plot generated: {contour_filename}")

contourPar(2553, 2, 2)
contourPar(5106, 2, 2)
contourPar(6385, 2, 2)

contourPar(2553, 2, 4)
contourPar(5106, 2, 4)
contourPar(6385, 2, 4)

contourPar(2553, 4, 4)
contourPar(5106, 4, 4)
contourPar(6385, 4, 4)