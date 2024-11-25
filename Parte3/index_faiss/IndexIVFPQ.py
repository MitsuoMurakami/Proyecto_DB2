import faiss
import numpy as np
import pickle
import time

N = 11903
k = 8

nlist = 100  # Numero de clusters
m = 16        # Cantidad de subespacios en PQ

with open("feature_spotify.pkl", "rb") as file:
    track_ids = pickle.load(file)
    features = pickle.load(file)

features_flat = np.vstack([np.array(f, dtype='float32') for f in features[:N] if len(f[0]) == 1280])

quantizer = faiss.IndexFlatL2(1280)  # Quantizador base        # distancia euclidiana
index_ivfpq = faiss.IndexIVFPQ(quantizer, 1280, nlist, m, 8)

# Indexacion
start_time = time.time()
index_ivfpq.train(features_flat)
index_ivfpq.add(features_flat)
indexation_time = time.time() - start_time
print(f"Tiempo de indexación (IVFPQ CPU): {indexation_time:.2f} segundos")

# Busqueda K-NN
def knn_search_ivfpq(query_index, k):
    query_vector = features_flat[query_index].reshape(1, -1)
    start_time = time.time()
    D, I = index_ivfpq.search(query_vector, k)
    search_time = time.time() - start_time
    print(f"Búsqueda k-NN:")
    print("Distancias:", D[0])
    print("Índices:", I[0])
    print(f"Tiempo de búsqueda k-NN: {search_time:.2f} segundos")
    return search_time

# Busqueda por rango
def range_search_ivfpq(query_index, radius):
    query_vector = features_flat[query_index].reshape(1, -1)
    start_time = time.time()
    lims, D, I = index_ivfpq.range_search(query_vector, radius)
    range_search_time = time.time() - start_time
    print(f"Búsqueda por rango:")
    print("Distancias:", D)
    print("Índices:", I)
    # for i in range(lims[0], lims[1]):
    #     print(f"Índice: {I[i]}, Distancia: {D[i]}")
    print(f"Tiempo de búsqueda por rango: {range_search_time:.2f} segundos")
    return range_search_time

query_index = 100
rad = 5.5*10**6
# rad = 50000000

# Ejecutar búsqueda K-NN
print()
knn_search_ivfpq(query_index, k)

# Ejecutar búsqueda por rango
print()
range_search_ivfpq(query_index, rad)
