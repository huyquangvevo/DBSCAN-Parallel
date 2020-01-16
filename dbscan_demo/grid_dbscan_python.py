import time
import warnings

import numpy as np
import matplotlib.pyplot as plt

from sklearn import cluster, datasets
from sklearn.preprocessing import StandardScaler
from itertools import cycle, islice
from tqdm import tqdm
np.random.seed(0)


def get_num_part(lowest, largest, cell_width):
    return int((largest - lowest) / cell_width) + 1


def Sequential_DBSCAN(data, X, most_left, most_bottom, cell_width, most_top, most_right, draw=1, lol=0):
    dbscan = cluster.DBSCAN(.3, 5)
    y_pred = dbscan.fit_predict(data)
    # print(y_pred)

    if draw:
        colors = np.array(list(islice(cycle(['#377eb8', '#ff7f00', '#4daf4a',
                                            '#f781bf', '#a65628', '#984ea3',
                                            '#999999', '#e41a1c', '#dede00']),
                                            int(4))))
        plt.scatter(X[:, 0], X[:, 1], s=10, color=colors[[3]*len(X)])
        plt.scatter(data[:, 0], data[:, 1], s=10, color=colors[y_pred])
        index = 0
        while most_bottom + index * cell_width < most_top:
            plt.axhline(y=most_bottom + index * cell_width)
            index += 1
        index = 0
        while most_left + index * cell_width < most_right:
            plt.axvline(x= most_left + index * cell_width)
            index += 1
        plt.savefig("vinh{}.png".format(lol))
        # draw = 0
        plt.close()
    # exit()
    return y_pred


class Group:
    def __init__(self, id):
        self.points = []
        self.group_id = id
    
    def add_point(self, point):
        self.points.append(point)
    
    def getmatrix(self, full_matrix):
        return full_matrix[self.points]


class Point:
    """
    cluster_id:
        0: Unclassified
        -1: Noise
    """
    def __init__(self, id, X_cor, Y_cor, most_left, most_right, most_bottom, most_top, cell_width, eps, num_cols):
        self.id = id
        self.X_cor = X_cor
        self.Y_cor = Y_cor
        
        self.cluster_id = []
        self.cell_id = -1 # Undefined
        self.row_index = -1
        self.col_index = -1
        self.groups = []
        self.is_core = None
        self.compute_cell_id(most_left, most_bottom, cell_width, num_cols)
        self.assign_cell_group(cell_width, most_left, most_right, most_bottom, most_top, eps, num_cols)
    

    def compute_cell_id(self, most_left, most_bottom, cell_width, num_cols):
        self.row_index = int((self.Y_cor - most_bottom) / cell_width)
        self.col_index = int((self.X_cor - most_left) / cell_width)
        self.cell_id = self.col_index + self.row_index * num_cols
        self.groups.append(self.cell_id)
        return self.cell_id


    def assign_cell_group(self, cell_width, most_left, most_right, most_bottom, most_top, eps, num_cols):
        if self.cell_id == -1:
            print("Determind cell_id first, exitting...")
            exit()
        
        right = self.X_cor + eps
        left = self.X_cor - eps
        bot = self.Y_cor - eps
        top = self.Y_cor + eps

        if right < most_right:
            if right >= (self.col_index + 1) * cell_width + most_left:
                self.groups.append(self.cell_id + 1)
        if left > most_left:
            if left <= (self.col_index) * cell_width + most_left:
                self.groups.append(self.cell_id - 1)
        
        if top < most_top:
            if top >= (self.row_index + 1) * cell_width + most_bottom:
                self.groups.append(self.cell_id + num_cols)
        if bot > most_bottom:
            if bot <= (self.row_index) * cell_width + most_bottom:
                self.groups.append(self.cell_id - num_cols)
        # if self.cell_id == 14:
            # import pdb
            # pdb.set_trace()
        if len(self.groups) == 3:
            self.groups.append(self.groups[1] + self.groups[2] - self.groups[0])


def get_distance(A, B):
    return np.sqrt(np.sum((A - B)**2))



def check_core(point_obj, groups_dict, eps, X, minPts):
    candidates = []
    for group_id in point_obj.groups:
        group = groups_dict[group_id]
        candidates += group.points
    count_eps_closure = 0
    for can_point in candidates:
        if 0 < get_distance(X[point_obj.id], X[can_point]) < eps:
            count_eps_closure += 1
    if count_eps_closure >= minPts:
        return True
    return False



def check_disjoin(cor_lists):
    for i in range(len(cor_lists) - 1):
        listi = cor_lists[i]
        for j in range(i + 1, len(cor_lists)):
            listj = cor_lists[j]
            # for ele in listi:


if __name__ == "__main__":
    n_samples = 150000
    eps=.3
    cell_width = 3 * eps
    minPts = 2
    X, y = datasets.make_circles(n_samples=n_samples, factor=0.5,
                                        noise=.03)
    X = StandardScaler().fit_transform(X)

    most_left = X[:, 0].min()
    most_right = X[:, 0].max()
    most_bottom = X[:, 1].min()
    most_top = X[:, 1].max()
    num_cols = get_num_part(most_left, most_right, cell_width)

    points_dict = {}
    groups_dict = {}
    for i in range(len(X)):
        if i % 100 == 0:
            print("Point: {}".format(i))
        new_point = Point(i, X[i][0], X[i][1], most_left, most_right, most_bottom, most_top, cell_width, eps, num_cols) 
        points_dict[i] = new_point
        groups = new_point.groups
        for gr in groups:
            if gr not in groups_dict.keys():
                new_group = Group(id=gr)
                new_group.add_point(i)
                groups_dict[gr] = new_group
            else:
                groups_dict[gr].add_point(i)
    
    cluster_dict = {}
    min_cluster_id = 0
    for gr_id, gr in groups_dict.items():
        # FOR EACH BLOCK
        y_pred_group = Sequential_DBSCAN(gr.getmatrix(X), X, most_left, most_bottom, cell_width, most_top, most_right, lol=gr_id)
        y_pred_group += min_cluster_id + 1
        # print(y_pred_group)
        min_cluster_id = y_pred_group.max()
        for index, point in enumerate(gr.points):
            cluster_id = y_pred_group[index]
            if cluster_id not in cluster_dict.keys():
                cluster_dict[cluster_id] = set([point])
            else:
                cluster_dict[cluster_id].add(point)
    
    not_seen = [1] * 3000
    for cluster_id, points_belong in cluster_dict.items():
        for point in points_belong:
            not_seen[point] = 0
            points_dict[point].cluster_id.append(cluster_id)

    merge_dict = {}
    # point_to_merges = []
    ahihi = []
    for point in tqdm(points_dict.keys()):
        # print(points_dict[point].cluster_id)
        point_obj = points_dict[point]
        if len(point_obj.cluster_id) > 1:
            # is_core = check_core(point_obj, groups_dict, eps, X, minPts)
            # print(point_obj.cell_id, point_obj.cluster_id)
            # if is_core:
            for id in point_obj.cluster_id:
                if id not in merge_dict.keys():
                    merge_dict[id] = [ele for ele in point_obj.cluster_id if ele != id]
                else:
                    merge_dict[id] += [ele for ele in point_obj.cluster_id if ele != id and ele not in merge_dict[id]]
            # else:
                # print("lol")
    
    for id in merge_dict:
        while True:
            list_id = merge_dict[id]
            old = len(list_id)
            for ele in list_id:
                list_ele = merge_dict[ele]
                list_id += [x for x in list_ele if x not in list_id and x != id]
            new = len(list_id)
            if new == old:
                break
            else:
                merge_dict[id] = list_id
    ids = list(merge_dict.keys()).copy()
    for id in ids:
        try:
            list_id = merge_dict[id]
            for ele in list_id:
                merge_dict.pop(ele)
        except:
            continue

    print("Merge dict")
    print(merge_dict)

    count = 0
    new_merge_dict = {}
    for id in merge_dict:
        new_merge_dict[id] = count
        for ele in merge_dict[id]:
            new_merge_dict[ele] = count
        count += 1
    print("new merge dict")

    print(new_merge_dict)

    X = []
    y = []
    for point in points_dict:
        point_obj = points_dict[point]
        X.append([point_obj.X_cor, point_obj.Y_cor])
        y.append(new_merge_dict[point_obj.cluster_id[0]])
    
    X = np.array(X)
    y = np.array(y)

    colors = np.array(list(islice(cycle(['#377eb8', '#ff7f00', '#4daf4a',
                                            '#f781bf', '#a65628', '#984ea3',
                                            '#999999', '#e41a1c', '#dede00']),
                                            int(y.max() + 1))))
    plt.scatter(X[:, 0], X[:, 1], s=10, color=colors[y])
    # plt.scatter(data[:, 0], data[:, 1], s=10, color=colors[y_pred])
    index = 0
    while most_bottom + index * cell_width < most_top:
        plt.axhline(y=most_bottom + index * cell_width)
        index += 1
    index = 0
    while most_left + index * cell_width < most_right:
        plt.axvline(x= most_left + index * cell_width)
        index += 1
    plt.show()
