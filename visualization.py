import matplotlib.pyplot as plt
import numpy as np 

X = []
Y = []
labels = []
with open("./data/partitions/8.txt","r") as f:
    points = f.readlines()
    for point in points:
        p = point.split(" ")
        X.append(float(p[0]))
        Y.append(float(p[1]))
        labels.append(int(p[2]))

# plt.plot(X,Y,'bo')
plt.scatter(X,Y,c=labels)
plt.show()        