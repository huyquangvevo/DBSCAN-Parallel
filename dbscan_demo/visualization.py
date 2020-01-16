import matplotlib.pyplot as plt
import numpy as np 

X = []
Y = []
labels = []
with open("../data/partitions/24.txt","r") as f:
    points = f.readlines()
    for point in points:
        p = point.split(" ")
        X.append(float(p[0]))
        Y.append(float(p[1]))
        labels.append(int(p[2]))

# plt.plot(X,Y,'bo')
l = set(labels)
print(len(l))
plt.scatter(X,Y,c=labels)
# plt.grid(True)
plt.grid(which='both')

plt.grid(which='major', alpha=0.25)
plt.show()        