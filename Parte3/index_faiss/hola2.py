import pickle

# load the data
with open("feature_spotify.pkl", "rb") as file:
    track_ids = pickle.load(file)
    features = pickle.load(file)

# print(track_ids)

print(len(features))

print(features[0])

# for i in features:
#     for j in i:
#         if len(j) != 1280:
#             print(len(j))
# print(features[1])
