#!/usr/bin/python3.6

import json
import os
import matplotlib.pyplot as plt
import numpy as np
import sys
import mplleaflet

# fitxer d'entrada
in_file = 'aceitunas.txt'

# nom del fitxer de sortida: el mateix que l'entrada, canviant extensio per html
mapfile = in_file.split('.')[-2]+".html"

# convertim les dades del fitxer en un array del numpy
latitud  = []
longitud = []

latitud1 = []
longitud1 = []

aux = 0

with open(in_file, 'r') as f:
    for line in f:
        if line.startswith("#"): # Ignorem les linies que comencen per #
            aux = 1
            continue
        fields = line.split('|')
        for field in fields:
            field.strip();
        if aux == 0:
            latitud1.append(float(fields[1]))
            longitud1.append(float(fields[2]))
        else:
            latitud.append(float(fields[1]))
            longitud.append(float(fields[2]))



xy = np.array([[longitud[i],latitud[i]] for i in range(len(latitud))])
#xy1 = np.array([[longitud1[i],latitud1[i]] for i in range(len(latitud1))])

# Dibuixem el cami amb punts vermells connectats per linies blaves
plt.plot(xy[:,0], xy[:,1], 'r.')
#plt.plot(xy1[:,0], xy1[:,1], 'g.', markersize=10)
plt.plot(xy[:,0], xy[:,1], 'b')
# plt.show()
# Creem el mapa i el guardem amb el nom guardat a mapfile
mplleaflet.show(path=mapfile)
