import pandas as pd

# Cargar el archivo CSV original
input_file = "spotify_songs.csv"
output_file = "spotify_songs2.csv"

# Leer la data del CSV
df = pd.read_csv(input_file)

# Filtrar canciones en inglés
filtered_df = df[df["language"] == "en"]

# Eliminar canciones con letras no deseadas
filtered_df = filtered_df[
    ~filtered_df["lyrics"].str.lower().isin(["instrumental", "na", "na na"])
]

# Guardar el resultado en un nuevo archivo CSV
filtered_df.to_csv(output_file, index=False)

print(f"Archivo filtrado guardado como {output_file}")
