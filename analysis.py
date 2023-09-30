import sys
import numpy as np
from sklearn.cluster import KMeans
from sklearn.decomposition import NMF
from sklearn.metrics import silhouette_score
import symnmf
import sys
import symnmfmodule as module
import pandas as pd

def kmeans(k, iter, input_data):
    # initialize centroids as first k datapoints:
    e = 0.001
    k = int(k)
    iter = int(iter)
    data = data_reader(input_data)
    # check requirements:
    check_requirements(k, iter, len(data))
    centroids = data[:k]
    # repeat until convergence:
    i = 0
    while i < iter:
        # assign every x in data to the closest cluster k:
        clusters_index = assign_to_cluster(centroids, data)
        # update the centroids:
        new_centroids = update_centroids(clusters_index, len(data[0]))
        # check if delta u_i < e:
        if convergence(centroids, new_centroids, e, k):
            centroids = new_centroids
            break
        centroids = new_centroids
        i = i + 1
    return data_writer(centroids)


def assign_to_cluster(centroids, data):
    # clusters_index[j] = list of all the elements from data assigned to centroids[j]
    clusters_index = [[] for i in range(len(centroids))]
    for i in range(len(data)):
        min_dis = float("inf")
        curr = 0
        for j in range(len(centroids)):
            dist = dis_calculator(data[i], centroids[j])
            if dist < min_dis:
                min_dis = dist
                curr = j
        clusters_index[curr].append(data[i])
    return clusters_index


def update_centroids(clusters_index, vector_length):
    new_centroids = []
    for cluster in clusters_index:
        tmp = [0 for j in range(vector_length)]
        for elem in cluster:
            for coordinate in range(vector_length):
                tmp[coordinate] += (elem[coordinate] / len(cluster))
        new_centroids.append(tmp)
    return new_centroids


def dis_calculator(p, q):
    dis = (sum((px - qx) ** 2.0 for px, qx in zip(p, q))) ** 0.5
    return dis


def convergence(centroids, new_centroids, e, k):
    for i in range(k):
        if dis_calculator(centroids[i], new_centroids[i]) >= e:
            return False
    return True


def check_requirements(k, iter, length):
    if k <= 1 or k >= length or not isinstance(k, int):
        print("Invalid number of clusters!")
        exit()
    if iter <= 1 or iter >= 1000 or not isinstance(iter, int):
        print("Invalid maximum iteration!")
        exit()


# convert data to lst of floats numbers
def data_reader(input_data):
    file = None
    try:
        file = open(input_data, 'r')
    except IOError:
        print("An Error Has Occurred")
        exit(1)
    data = []
    file_reader = file.readlines()
    file.close()
    for vector_line in file_reader:
        data.append([float(i) for i in vector_line.split(",")])
    return data


def data_writer(centroids):
    # for c in centroids:
    #     string = ""
    #     for i in c:
    #         string += '%.4f' % i
    #         if i == c[-1]:
    #             print(string)
    #         else:
    #             string += ","
    return centroids


# Function to assign each data point to the nearest centroid
def assign_to_nearest_centroid(data, centroids):
    assignments = []
    for point in data:
        # Calculate distances from the data point to all centroids
        distances = [np.linalg.norm(point - centroid) for centroid in centroids]
        # Find the index of the nearest centroid
        nearest_centroid_index = np.argmin(distances)
        # Assign the data point to the nearest centroid
        assignments.append(nearest_centroid_index)
    return assignments


def main():
    v_length = len(sys.argv)
    if v_length == 4:
        k = sys.argv[1]
        max_iter = sys.argv[2]
        input_arg = sys.argv[3]
    else:
        k = sys.argv[1]
        input_arg = sys.argv[2]
        max_iter = '200'

    data = data_reader(input_arg)
    data_np = np.array(data)
    data_1 = pd.read_csv(input_arg, header=None)
    data_1 = data_1.to_numpy()
    d = np.array(data_1).flatten().tolist()

    # symnmf
    # Convert H matrix into hard cluster assignments
    num_of_rows = int(data_np.shape[0])
    num_of_columns = int(data_np.shape[1])
    H = symnmf.create_H(int(k), d, num_of_rows, num_of_columns)
    hard_cluster_assignments = np.argmax(H, axis=1)
    silhouette_avg = silhouette_score(data_np, hard_cluster_assignments)
    # Print the Silhouette Score
    print(f"nmf: {silhouette_avg:.4f}")

    # Kmeans
    km = kmeans(k, max_iter, input_arg)
    # Get cluster assignments for each data point
    kmeans_assignments = assign_to_nearest_centroid(data_np, km)
    # Flatten the list of lists into a single list of cluster labels
    kmeans_labels = []
    for i, cluster in enumerate(km):
        kmeans_labels.extend([i] * len(cluster))
    # Calculate the silhouette score for K-means
    kmeans_silhouette_score = silhouette_score(data_np, kmeans_assignments)
    print(f"kmeans: {kmeans_silhouette_score:.4f}")


if __name__ == "__main__":
    main()
#
# if __name__ == "__main__":
#     if len(sys.argv) != 3:
#         print("Usage: python3 analysis.py <k> <file_name>")
#         sys.exit(1)
#
#     k = int(sys.argv[1])
#     file_name = sys.argv[2]
#
#     data = read_data(file_name)
#
#     # Perform SymNMF clustering
#     symnmf_clusters = symnmf_clustering(data, k)
#     symnmf_score = silhouette_score(data, symnmf_clusters)
#
#     # Perform K-means clustering
#     kmeans_clusters = kmeans_clustering(data, k)
#     kmeans_score = silhouette_score(data, kmeans_clusters)
#
#     print(f"nmf: {symnmf_score:.4f}")
# #     print(f"kmeans: {kmeans_score:.4f}")
# def custom_kmeans_clustering(data, k):
#     # Apply your custom K-means implementation
#     # Replace the following line with your custom K-means clustering logic
#     cluster_assignments = ...  # Calculate cluster assignments using your custom K-means
#     return cluster_assignments
#
# if __name__ == "__main__":
#     if len(sys.argv) != 3:
#         print("Usage: python3 analysis.py <k> <file_name>")
#         sys.exit(1)
#
#     k = int(sys.argv[1])
#     file_name = sys.argv[2]
#
#     data = read_data(file_name)
#
#     # Perform SymNMF clustering
#     symnmf_clusters = symnmf_clustering(data, k)
#     symnmf_score = silhouette_score(data, symnmf_clusters)
#
#     # Perform custom K-means clustering
#     custom_kmeans_clusters = custom_kmeans_clustering(data, k)
#     custom_kmeans_score = silhouette_score(data, custom_kmeans_clusters)
#
#     print(f"nmf: {symnmf_score:.4f}")
#     print(f"kmeans: {custom_kmeans_score:.4f}")
