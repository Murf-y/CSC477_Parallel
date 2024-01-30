# read xlsx
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_excel('./runtime.xlsx', sheet_name='Runtime')


# number of files
x = df.iloc[:, 0].values

# number of cores
y = np.arange(1, 17)

# create 2D grid for x and y
X, Y = np.meshgrid(x, y)

# for each number of files, get the running time for all cores (1, 2, ...)
Z = df.iloc[:, 1:17].values.T

# 3D plot
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Plot a 3D surface
ax.plot_surface(X, Y, Z, cmap=plt.cm.viridis, linewidth=0.2)

# Add labels
ax.set_xlabel('Number of files')
ax.set_ylabel('Number of cores')
ax.set_zlabel('Running time')

plt.show()

# 2D plot
Z = Z.T

fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))

for i, number_of_file in enumerate(df.iloc[:, 0].values):
    ax1.plot(y, Z[i], label=str(number_of_file) + ' files')
    ax2.plot(y, np.log(Z[i]), label=str(number_of_file) + ' files')

ax1.legend()
ax1.set_xlabel('Number of cores')
ax1.set_ylabel('Running time')
ax1.set_title("Normal Scale")

ax2.legend()
ax2.set_xlabel('Number of cores')
ax2.set_ylabel('LOG Running time')
ax2.set_title("LOG Normal Scale")

plt.show()
