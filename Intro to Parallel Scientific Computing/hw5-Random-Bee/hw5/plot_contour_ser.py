import numpy as np
import matplotlib.pyplot as plt

# if plots directory does not exist, create it
import os
if not os.path.exists('plots'):
    os.makedirs('plots')

def contourSer(tid):
    file_name = f'T_x_y_{tid:06d}.dat'

    # Read data
    a = np.loadtxt(file_name)

    # Reshape data
    n = int(np.sqrt(a.shape[0]))
    x = a[::n, 0]  # Take every nth element from first column
    y = a[:n, 1]   # Take first n elements from second column
    T = a[:, 2].reshape((n, n))  # Reshape the third column to nxn

    # Plot contour
    plt.figure()
    plt.contourf(x, y, T.T, cmap='jet')
    plt.xlabel('x')
    plt.ylabel('y')
    plt.title(f't = {tid:06d}, Serial')
    plt.xlim([-0.05, 1.05])
    plt.ylim([-0.05, 1.05])
    plt.clim([-0.05, 1.05])
    plt.colorbar()
    plt.gca().tick_params(labelsize=14)
    contour_filename = f'plots/cont_T_{tid:04d}.png'
    plt.savefig(contour_filename, dpi=300)
    
    plt.close('all')
    
    print(f'Plot generated: {contour_filename}')

contourSer(2553)
contourSer(5106)
contourSer(6385)