import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
from matplotlib.colors import LinearSegmentedColormap

#Color map
cm_data = np.loadtxt("./ScientificColourMaps3/ScientificColourMaps3/berlin/berlin.txt")
CBname_map = LinearSegmentedColormap.from_list('CBname', cm_data)

data = pd.read_table('PrimaryFlux.txt', sep=",",comment='#',index_col='Name')
time = 2*3600
fig=plt.figure()
ax=plt.gca()
plt.scatter(data.Z,data.Flux/time,c=data.A,cmap=CBname_map)
ax.set_yscale('log')
for i, label in enumerate(data.index):
    ax.annotate(label, (data.Z[i],data.Flux[i]/time))
plt.xlabel('Atomic Number Z')
plt.ylabel(r'Number of Primary Particles [$\frac{No.Particles}{s*m^2}$]')
plt.title('Primary Particle Flux')
plt.xlim(0,30)
plt.ylim(1e-4,1e4)
#plt.grid(True)
colorbar=plt.colorbar()
colorbar.set_label('Atomic Mass')
fig.savefig('PrimaryFlux_v1.pdf')