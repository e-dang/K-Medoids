from sklearn.datasets import make_blobs
import matplotlib.pyplot as plt
from array import array
import struct
import numpy as np
import matplotlib.cm as cm


def generate_data(num_data, num_features, num_clusters, cluster_std, box, data_fp, labels_fp):
    data, labels = make_blobs(n_samples=num_data, n_features=num_features,
                              centers=num_clusters, cluster_std=cluster_std, center_box=box)

    #  write data
    with open(data_fp, 'wb') as file:
        for point in data:
            point_array = array('d', point)
            point_array.tofile(file)

    # write labels
    with open(labels_fp, 'wb') as file:
        label_array = array('l', labels)
        label_array.tofile(file)

    return data


def read_data(filepath, num_data, num_features, data_format='d'):
    vals = [[] for _ in range(num_data)]
    with open(filepath, 'rb') as file:
        data = file.read(8)
        i = 0
        while data:
            ind = i // num_features
            vals[ind].append(struct.unpack(data_format, data)[0])
            data = file.read(8)
            i += 1
    ret = np.concatenate([np.array(i) for i in vals]).reshape((-1, num_features))
    return ret[~np.isnan(ret)].reshape((-1, 2))


def read_clustering(filepath):
    vals = []
    with open(filepath, 'rb') as file:
        data = file.read(4)
        while data:
            vals.append(struct.unpack('i', data)[0])
            data = file.read(4)

    return vals


def plot_data(data, clusters, clustering):
    colors = cm.jet(np.linspace(0, 1, len(clusters)))
    new_colors = [colors[i] for i in clustering]
    plt.scatter(data[:, 0], data[:, 1], c=new_colors)
    plt.scatter(clusters[:, 0], clusters[:, 1], c='black')
    plt.show()


NUM_DATA = 10000
NUM_FEATURES = 2
NUM_CLUSTERS = 10
CLUSTER_STD = 6
BOX = (-100, 100)


'''
UNCOMMENT OUT THIS SECTION TO GENERATE DATA
'''
# data = generate_data(NUM_DATA, NUM_FEATURES, NUM_CLUSTERS, CLUSTER_STD, BOX,
#                      f'test_{NUM_DATA}_{NUM_FEATURES}.txt', f'data_labels_{NUM_DATA}_{NUM_FEATURES}.txt')

'''
UNCOMMENT OUT THIS SECTION TO PLOT CLUSTERING RESULTS
'''
# num = 10000
# dims = 2
# file_num = 0
# data = read_data(f'test_{num}_{dims}.txt', num, dims)
# clusters = read_data(f'test_{num}_{dims}_clusters_{file_num}.txt', num, dims)
# clustering = read_clustering(f'test_{num}_{dims}_clustering_{file_num}.txt')
# plot_data(data, clusters, clustering)
