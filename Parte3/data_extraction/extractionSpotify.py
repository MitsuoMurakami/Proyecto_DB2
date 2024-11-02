import spotipy
import credentials
import numpy as np
import pandas as pd
import os
import requests
from spotipy.oauth2 import SpotifyOAuth

"""
The code stops working by many request to the API.
"""

spAuth = SpotifyOAuth(
    client_id=credentials.client_id,
    client_secret=credentials.client_secret,
    redirect_uri=credentials.redirect_url, # add to the dashboard to accept the redirect
    scope= credentials.scope
)

# Get token info
# token = spAuth.get_access_token()
# isExpired = spAuth.is_token_expired(token)
# print(isExpired)


sp = spotipy.Spotify(auth_manager=spAuth)

# ids de songs
df_songs = pd.read_csv('./spotify_songs.csv')
df_songs = df_songs['track_id']
df_songs = df_songs.to_list()
df_songs = df_songs[4943:]
# carpeta para guardar las muestras de audio
output_folder = "data_previews"
os.makedirs(output_folder, exist_ok=True)

# muestras de 30 segundos
success = 0
fail = 0
i = 4943
for track_id in df_songs:
    # Obtener información de la canción
    track_info = sp.track(track_id)
    preview_url = track_info.get('preview_url')
    track_name = track_info['name']
    if preview_url:
        # descargar la muestra
        try:
            response = requests.get(preview_url)
            file_path = os.path.join(output_folder, f"{track_name}_{track_id}.mp3")
            # guardar el archivo
            with open(file_path, 'wb') as f:
                f.write(response.content)
            print(f"Descargada: {i} {track_name}")
            success += 1
        except Exception as e:
            print(f"Error: {i} {track_name}: {e}")
            fail += 1
    else:
        print(f"No hay: {i} {track_name}")
        fail += 1
    i += 1

print(f"Descargadas {success} canciones, fallaron {fail}")

#_-----------------------------------------------------------
# NO FUNCIONO DESCARGAR LA PLAYLIST ENTERA, SON MUCHOS ARCHIVOS
# # split df_songs half
# df_songs1 = np.array_split(df_songs, 2)[0]
# df_songs2 = np.array_split(df_songs, 2)[1]
# # id de playlist
# playlist_id1 = "0cgFimMHJrI4wdF8xb2LkV"
# playlist_id2 = "0LIMHzPnHEWzzpCW7r4aFL"

# # dividir la lista de canciones en bloques de 100
# def split_into_chunks(lst, n):
#     for i in range(0, len(lst), n):
#         yield lst[i:i + n]
#
# # add songs a la playlist en bloques de 100
# for chunk in split_into_chunks(df_songs1, 100):
#     sp.playlist_add_items(playlist_id1, chunk)
#     print(f"Agregados {len(chunk)} canciones a la playlist.")
#
# for chunk in split_into_chunks(df_songs2, 100):
#     sp.playlist_add_items(playlist_id2, chunk)
#     print(f"Agregados2 {len(chunk)} canciones a la playlist.")



