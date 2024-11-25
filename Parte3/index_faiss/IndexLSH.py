import faiss
import numpy as np
import pickle
import time

N = 11903  # Número de canciones (features)
k = 8       # Número de vecinos más cercanos

with open("feature_spotify.pkl", "rb") as file:
    track_ids = pickle.load(file)
    features = pickle.load(file)

features_flat = np.vstack([np.array(f, dtype='float32') for f in features[:N] if len(f[0]) == 1280])

n_bits = 1024  # Número de bits para el hash LSH
index_lsh = faiss.IndexLSH(1280, n_bits)

# Indexacion
start_time = time.time()
index_lsh.add(features_flat)
indexation_time = time.time() - start_time
print(f"Tiempo de indexación (LSH CPU): {indexation_time:.2f} segundos")

# Busqueda K-NN
def knn_search_lsh(query_index, k):
    query_vector = features_flat[query_index].reshape(1, -1)
    start_time = time.time()
    D, I = index_lsh.search(query_vector, k)
    search_time = time.time() - start_time
    print(f"Búsqueda k-NN para el objeto en el índice {query_index} (k={k}):")
    print("Distancias:", D[0])
    print("Índices:", I[0])
    print(f"Tiempo de búsqueda k-NN: {search_time:.2f} segundos")
    return search_time




query_index = 100

print()
# Ejecutar búsqueda K-NN
knn_search_lsh(query_index, k)
