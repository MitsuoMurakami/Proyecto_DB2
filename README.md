# Proyecto BD2 - Búsqueda y Recuperación de la Información

## Tabla de Contenidos
- [Introducción](#introducción)
- [Backend: Índice Invertido](#backend-índice-invertido)
- [Backend: Índice Multidimensional](#backend-índice-multidimensional)
- [Frontend](#frontend)
- [Experimentación](#experimentación)

## Introducción
### Objetivo del Proyecto
El objetivo de nuestro proyecto es implementar un sistema de indexación para canciones de Spotify con búsquedas por similitud rapidas y eficientes. Para ello, se usará modelos de recuperación y búsqueda con la técnica del índice invertido como método de indexación. El usuario escribirá 1 o más palabras y utilizando un índice invertido en memoria secundaria, se retornará las canciones que tengan la mayor similitud. Asimismo, el usuario podrá ingresar archivos de audio y se le retornará los audios más similares usando estructuras multidimensionales. 
Además, para una mejor experiencia para el usuario, implementaremos una interfaz gráfica en donde el usuario pueda ingresar datos textuales de la canción o archivos de audio para obtener los resultados de manera más amigable. 

### Dominio de Datos e Importancia de la Indexación

El Índice Invertido en memoria secundaria (Parte 1) se realizará utilizando [Audio features and lyrics of Spotify songs dataset](https://www.kaggle.com/datasets/imuhammad/audio-features-and-lyrics-of-spotify-songs/data), el cual contiene información de 18455 canciones de Spotify. Dicho dataset contiene la siguiente información: 

* **Datos básicos de la canción:** id de la canción, nombre, artista, letras, popularidad y fecha de lanzamiento.

* **Información del álbum:** nombre, id del álbum y su fecha de lanzamiento.

* **Detalles de la playlist:** cada canción puede estar en una o varias playlists, cada una con un id, nombre, género y subgénero

* **Atributos de audio:** como la bailabilidad, energía, tono, modo (mayor o menor), y el grado de vocalización del cantante.

* **Características compositivas:** la acústica, los instrumentos utilizados, la vivacidad de la canción  y la positividad que se transmite.

* **Duración y lenguaje:** También tenemos la duración de la canción y el idioma en que se canta

Por otro lado, el Índice Multidimensional (Parte 2) se realizará utilizando los previews de las canciones presentes en Audio features and lyrics of Spotify songs dataset. Estos fueron obtenidos usando la API de Spotify y la libreria Spotipy, dado que no todas las canciones tenian un preview disponible, se obtuvo un total de 11883 audios de 30 segundos.

### Importancia de la indexacion:

El manejo de grandes volúmenes de datos es un problema muy presente en la actualidad, más aún cuando se trata de datos no estructurados como los audios de las canciones o datos con una gran cantidad de palabras como las letras de las canciones. En este contexto, la indexación es muy importante para poder realizar búsquedas eficientes y rápida. Es asi que, empresas como Spotify que manejan una gran cantidad de canciones, necesitan de un buen sistema de indexación para poder realizar búsquedas y hacer recomendaciones de canciones con base en su similitud. Por ello, en este proyecto se implementará un sistema de indexación que permita realizar búsquedas eficientes y rápidas en grandes volúmenes de datos.


## Backend: Índice Invertido (Parte 1)

### Construcción del Índice Invertido SPIMI
#### Construcción del índice invertido en memoria secundaria.

La implementación del índice invertido se hizo en 2 etapas, la creación los SPIMI blocks y el proceso de merge de los bloques para la generación de un índice en memoria secundaria.

En la siguiente imagen, se puede observar la primera etapa, esta comienza con un archivo .csv que contiene información de 18455 canciones, cada una con 25 atributos. Dado que este es un archivo grande, el preprocesamiento se hizo por una canción a la vez, es decir, las líneas del archivo .csv se extrajeron una por una. De cada canción solo se consideraron los atributos de nombre, artista y letra. Estos atributos se concatenaron y se les aplicó un proceso de tokenización, filtro de stopwords y reducción de palabras. Dado que el archivo .csv content canciones en diferentes idiomas, el preprocesamiento se hizo de acuerdo al idioma de cada canción, para lo cual se empleó 15 stoplist diferentes.

Luego del preprocesamiento de cada canción se obtuvo una lista de tokens, con los cuales se calculó el term frequency (tf) de cada token. Con ello, cada vez que se preprocesaba una canción, se escribia en un el tf de los tokens y el id relativo de la canción a la que pertencen en un archivo temporal preprocessed_word.txt.

Por último, para la creación de los SPIMI blocks, se recorrió el archivo preprocessed_word.txt y se aplicó el algoritmo SPIMI. Este algoritmo consiste en leer el archivo preprocessed_word.txt y crear un índice invertido con un diccionario, sin embargo, dado que se tiene memoria limitada, cada vez que dicho diccionario alcanza un tamaño de 0.5 Mb, se ordena alfabéticamente y se almacena en un bloque. Cada bloque se guarda en un archivo temporal con el formato block_x.txt, donde x es el número de bloque. Cada bloque contiene un conjunto de pares (término, posting list) donde la posting list contiene los id de la canción y el tf de cada término en la canción.

![BD2_P2_1 drawio](https://github.com/user-attachments/assets/5b88dffa-f32e-4ea2-916e-44ae67b9d1ba)

La siguiente etapa consta del proceso de merge de los bloques obtenidos, en la siguiente figura se puede observar dicho proceso. Para ello, se abren todos los bloques creados y se lee el primer elemento de cada bloque. Estos elementos se insertan en un MinHeap para tener en el top al menor término alfabéticamente, este MinHeap tiene como tamaño la cantidad de bloques. 

Luego, se extrae el término del top del MinHeap y se compara con el siguiente termino en ser extraído, si son iguales se unen las posting list, si no se guarda el término en un archivo index.txt y se extrae el siguiente elemento del bloque del que se extrajo el término. Además, antes de escribir cada término, dado que ya se tiene todos los ids de las canciones en que aparece cada término, se calcula el idf y con ello se va calculando progresivamente la norma para cada documento. Este proceso se repite hasta que se hayan leído todos los términos de todos los bloques, es decir, hasta que el MinHeap tenga tamaño 0. 

Asimismo, cada vez que se escribe un término en el archivo index.txt, se guarda la posición física en la que se escribe el término y su posting list en un archivo position_terms.pkl. De esta manera, los terminos son escritos en un archivo index.txt alfabéticamente y se genera un índice invertido en memoria secundaria. 

![BD2_P2_2 drawio](https://github.com/user-attachments/assets/01a6fd22-1b83-438e-84f7-5e98e2bcd654)


### Construcción del índice invertido en PostgreSQL:
   1. Primero, agregamos una columna full_text en la tabla Canciones para almacenar la información concatenada del nombre de la canción, el nombre del artista y la letra. Esta columna facilita el análisis y búsqueda textual sobre múltiples atributos.
   2. Creamos un atributo de tipo vectorial, en este caso nosotros convertimos la columna(full_text) en la que concatenamos el nombre de la canción, el nombre del artista y la letra de la canción. PostgreSQL optimiza las palabras de la columna para una búsqueda eficiente mediante procesamiento lingüístico (por ejemplo, eliminando palabras comunes y aplicando lematización).
   3. Luego Creamos el índice GIN sobre dicho vector. GIN está diseñado para manejar casos en los que los elementos que se van a indexar son valores compuestos y las consultas que debe manejar el índice deben buscar valores de elementos que aparecen dentro de los elementos compuestos. Por ejemplo, los elementos podrían ser documentos y las consultas podrían ser búsquedas de documentos que contengan palabras específicas.
Un índice GIN almacena un conjunto de pares (clave, lista de contabilización), donde una lista de contabilización es un conjunto de identificadores de fila en los que aparece la clave.
   4. Por último aplicamos las consultas con el índice y la query usando como score la similitud coseno retornándonos un top K, teniendo en cuenta que las palabras de la query tiene que estar separada por el símbolo de or (|).

## Backend: Índice Multidimensional (Parte 2)

### Técnicas de Indexación
- Descripción de las librerías utilizadas

   * Faiss es un repositorio de Facebook el cual contempla varios tipos de indexación.
   * R-tree proporciona una serie de funciones avanzadas de indexación espacial para los usuarios de Python

- Justificación de la elección

   * Se utilizó IndexLSH debido a la alta dimensionalidad de la data. Este índice utiliza un método de cálculo de distancia de bajo costo, Hamming distance. Además de utilizar vectores binarios.

#### Preprocesamiento y Extracción de Características
Se comenzo haciendo el preprocesamiento de los audios de 11883 canciones, cada uno de 30 segundos. Estos archivos de audio se procesaron con Librosa y se uso Mel Frequency Cepstral Coefficient (MFCC) para extraer los feature vectors de las canciones. Con ello, se obtuvo 20 descriptores locales por cada canción, cada uno de dimension de 1280. 


Para las búsquedas del KNN con heap y KNN_R_tree se aplicó PCA para reducir la dimensionalidad, ya que cuando la dimensionalidad es muy grande estos modelos se hacen muy deficientes, es por eso que después de la recolección de vectores característicos y su respectiva normalización, aplicamos PCA y buscamos la cantidad de dimensiones que explican el 90% de la varianza. Logrando asi, que cada descriptor local sea reducido a solo 154 dimensiones.

#### KNN Search
- Implementación
La búsqueda KNN se implementó utilizando colas de prioridad basada en un max-heap para identificar las canciones más cercanas en donde:
Primero calculamos la distancia con cada vector característico de las canciones, gracias a esto podemos hacer el max-heap, para luego extraer los índices de los vecinos más cercanos, dividiendolo por las characteristics que en nuestro caso son 20. Finalmente, usamos majority voting y contamos cuantas veces aparece cada identificador para obtener el top_k vecinos más cercanos.
- Complejidad computacional
Donde N es: número de canciones que tenemos.
D: las dimensiones que contiene cada canción
O(log(k)) es el costo del heap donde k es el tamaño del heap
Total: O(N x D) + O(N x log(k))
- Optimizaciones realizadas
En cuanto a las optimizaciónes relizadas como se indicó usamos normalización y PCA para una mayor eficiencia, a parte de esto aplanamos las características para poder mapear los índices de las canciones y usar 1 KNN en vez de 20, después usamos el max-heap gracias a las colas de prioridad que nos ahorra bastantes operaciones en memoria y finalmente usamos un sistema de votación para obtener el top_k.

![image](https://github.com/user-attachments/assets/e2c73fcc-cfaf-40b3-9632-9dbe86480af6)

#### Indexacion R_tree
![image](https://github.com/user-attachments/assets/a5a02885-cc22-4c5b-b36c-f96a3aec7a4b)
#### KNN R_tree
![Knn_R-tree](https://github.com/user-attachments/assets/5f49ff37-38a2-4e8b-a7d8-19e119ed0621)

##### Maldición de la Dimensionalidad
- Análisis del problema

   * El Rtree no es eficiente por el alto solapamiento que ocurre al aplicar este índice.
   * Esto surge al trabajar con vectores de características que tienen una alta dimensionalidad (154 dimensiones para describir canciones). Esto puede afectar el desempeño del índice R-Tree y la búsqueda KNN debido al aumento de la complejidad del cálculo de distancias y la degradación de las métricas.
   * En un espacio de alta dimensionalidad, los datos tienden a dispersarse, lo que reduce la densidad de datos útiles en cualquier región del espacio.
   * Los cálculos de distancias y la búsqueda de vecinos más cercanos son computacionalmente más costosos en espacios de alta dimensionalidad.


- Estrategias de mitigación implementadas

   * Se aplicó PCA para reducir las dimensiones originales de los vectores de características a un espacio de menor dimensionalidad. Esto ayuda a conservar la mayor cantidad de información relevante posible mientras se elimina el ruido y las redundancias.
   * Dado que cada canción tiene 20 vectores de características, se realiza una búsqueda KNN para cada uno de estos vectores. Los vecinos más cercanos se cuentan y, a través de una votación mayoritaria, se determina cuáles son las canciones más similares. Este enfoque mejora la robustez al ruido y reduce el impacto de anomalías en vectores individuales.

#### KNN-HighD

- Se utilizó la librería faiss para el método de indexación LSH. Es un indice que indexa y realiza búsquedas en un tiempo bajo. Es una técnica approximate nearest neighbor (ANN) muy utilizada por grandes empresas por la rapidez que tiene.

- Al índice LSH se le pasan los datos como vectores donde n_bits es un parámetro utilizado para mapear estos features de entrada en la función de hashing. Este valor es para indicar la cantidad de bits que representan a cada feature, de tal manera que si es menor a la dimensión de la data original, se estaría perdiendo información, sin embargo, faiss utiliza el método de vectores ortogonales. Si n_bits es mayor a la dimensión original, se utiliza el método de marco ajustado ya que este es una técnica representativa (binaria) que mantiene las relaciones sin perder información importante. Esto ayuda a que el cálculo con Hamming distance sea más eficiente, donde se calcula la cantidad de diferencias entre bits de los vectores.

![lsh drawio](https://github.com/user-attachments/assets/d98efdf8-3c55-4c09-aedf-479d56fcc468)


## Frontend

Tecnologías utilizadas: El desarrollo de la interfaz se hizo con Gradio por su facilidad de uso e integracion en Python. Ademas, Gradio proporciona bloques predefinidos para reproducir audio, mostrar texto, entre otros, lo que facilitó el desarrollo de la interfaz.

### Manual de Usuario
1. Instalación: Para instalar las librerías necesarias crear un environment con conda y ejecutar el siguiente comando:

```bash
conda create -n myenv python=3.8
conda activate myenv
pip install -r requirements.txt
conda install -c pytorch faiss-cpu=1.9.0
```
2. Ejecución: Para ejecutar la interfaz, ejecutar todos los bloques los Jupyter Notebooks de la carpeta Parte2/SPIMI_GIN.ipynb y Parte3/R-tree/KNN_s.ipynb. Finalmente, abrir el navegador y dirigirse a la dirección http://localhost:7860/ para acceder a la interfaz.
3. Uso:

   * **Parte2/SPIMI_GIN.ipynb**: 
     * Las consultas en lenguaje natural para obtener canciones similares se realizan ingresando un texto en el cuadro 'Query' de SPIMI y presionando el botón "Get results". Se mostrarán las canciones más similares en la tabla de resultados.
     * Las consultas usando sintaxis de PostgreSQL se realizan ingresando un texto en el cuadro 'Query' de GIN y presionando el botón "Get results". Se mostrarán las canciones más similares en la tabla de resultados. En *select* se coloca las columnas que se quiere visualizar, en *liketo* la consulta y en *limit* la cantidad de resultados requeridos. En la siguiente tabla se muestra un ejemplo de la sintaxis a usar:
     * Columnas disponibles: track_id, track_name, track_artist, lyrics, track_popularity, track_album_id, track_album_name, track_album_release_date, playlist_name, playlist_id, playlist_genre, playlist_subgenre, danceability, energy, key, loudness, mode, speechiness, acousticness, instrumentalness, liveness, valence, tempo, duration_ms, language.
     * Ejemplo de sintaxis:
          ```sql
             select track_name,track_artist,lyrics from Canciones 
             where lyric liketo 'She played the fiddle in an Irish band But she fell in love with an English man Kissed' 
             limit 10;
          ```

   * **Parte3/R-tree/KNN_s.ipynb**:
     * En esta interfaz se puede reproducir cualquiera de las más de 10000 canciones disponibles usando el reproductor Song Preview. 
     * Para realizar una consulta acerca de la canción que se está reproduciendo, se debe ingresar una query con sintaxis SQL. Esta consulta tiene la siguiente estructura: en *select* se coloca las columnas que se quiere visualizar (diseñado para soportar unicamente song_id), en *from* se coloca la tabla de la base de datos 'spotify_songs', en *using* se coloca el KNN que se quiere usar, en *where* se coloca el nombre de la cancion buscada y en *limit* la cantidad de resultados requeridos.
     * Los nombres de los KNN soportados son: knn_search_lsh, knn_top_R_tree, knn_top_lineal.
     * Ejemplo de sintaxis:
          ```sql
             select song_id from spotify_songs using knn_search_lsh where song_name = '$20 Fine' LIMIT 5
          ```
     * Una vez que se tenga la query, colocarla en el cuadro de texto 'Search Query' y presionar el botón "Get results". Se mostrarán los nombres de las canciones más similares en la tabla de resultados.
### Capturas de Pantalla
[Incluir capturas de pantalla relevantes con descripciones]

#### Indice invertido: Interfaz para consultas
![Screenshot 2023-12-01 195517](https://github.com/user-attachments/assets/b526db19-73f5-4839-ac6d-231af5fdcd6f)

#### Índice invertido: Ejemplo de consulta y respuesta
![Screenshot 2023-12-01 195624](https://github.com/user-attachments/assets/fe7e652e-3f44-4384-b641-8119c301c537)


#### Indice multidimensional: Interfaz para consultas
![Screenshot 2024-12-02 030908](https://github.com/user-attachments/assets/b5b8ec91-625f-4ccf-970d-4d6c351d6d7a)

#### Índice multidimensional: Ejemplo de consulta y respuesta
![Screenshot 2024-12-02 030817](https://github.com/user-attachments/assets/a7e1da7a-eec6-4659-83ee-ab51fc63fed7)

## Experimentación

### Resultados Experimentales
#### Tablas de Resultados
Tabla sobre la medicion del performance entre SPIMI (implementacion propia) y PostgreSQL

|    N     | SPIMI   | GIN    |
|----------|---------|--------|
| 1000     | 0.5931  | 0.0097 |
| 2000     | 0.5718  | 0.0152 |
| 4000     | 0.5843  | 0.0558 |
| 6000     | 0.5805  | 0.035  |
| 8000     | 0.5631  | 0.0456 |
| 10000    | 0.6636  | 0.054  |
| 12000    | 0.5873  | 0.0637 |
| 14000    | 0.5862  | 0.0739 |
| 16000    | 0.5829  | 0.0861 |
| 18000    | 0.6367  | 0.0983 |


Tabla sobre el tiempo de ejecución entre KNN-RTree, KNN-secuencial y el KNN-HighD sobre una colección de objetos de
tamaño N con el valor de K = 8

|    N     | KNN secuencial | KNN-Rtree | KNN-HighD |
|----------|----------------|-----------|------------
| 700      |      1.7       |    1.4    |   0.0015  | 
| 1000     |      2.2       |    2.2    |   0.0022  |
| 3000     |      6.9       |    4.4    |   0.0032  |
| 5000     |      7.9       |    7.6    |   0.0049  |
| 7000     |      9.8       |    11.2   |   0.0053  |
| 9000     |      11.7      |    28.2   |   0.0055  |
| 11000    |      15.6      |    35.8   |   0.003   |

#### Gráficos Comparativos

##### SPIMI vs PostgreSQL
![image](https://github.com/user-attachments/assets/66f8eed6-7f25-48a9-9222-7a07cbdf6066)

##### KNN-RTree vs KNN-secuencial vs KNN-HighD
![image](https://github.com/user-attachments/assets/b3c6fdc3-1509-4408-8992-ee29cb90cdc3)


### Análisis y Discusión

   + PostgreSQL vs SPIMI: PostgreSQL demuestra ser superior, gracias a sus optimizaciones internas. Por otro lado, SPIMI, cumple su propósito de manera funcional y demuestra ser una solución sólida en colecciones pequeñas y medianas.
   + KNN-HighD se beneficia del uso de LSH implementado con Faiss, lo que permite búsquedas rápidas y eficientes en alta dimensionalidad, sin necesidad de reducir las dimensiones. Esto demuestra la capacidad de los métodos basados en hashing para superar las limitaciones de técnicas tradicionales.
   + KNN-RTree, aunque efectivo para pequeñas colecciones, no maneja bien las dimensiones altas, destacando una limitación inherente a los índices espaciales. 
### Conclusiones principales
   + Para grandes colecciones, KNN-HighD con LSH es el enfoque más eficiente, mientras que los métodos secuenciales y basados en R-Tree son significativamente más lentos. Esto subraya la importancia de elegir técnicas especializadas para escenarios de alta dimensionalidad.
   + KNN-RTree presenta limitaciones en alta dimensionalidad, siendo más adecuado para colecciones pequeñas o datos con pocas dimensiones.
   + PostgreSQL supera ampliamente a SPIMI en términos de tiempo de ejecución y escalabilidad, gracias a su infraestructura optimizada para búsqueda e indexación.
### Recomendaciones futuras
   + Experimentar con diferentes configuraciones de Faiss, como el número de buckets o el tipo de hashing.
   + Investigar variantes como X-Tree o R-Tree*, que son más robustas en alta dimensionalidad.
   + Ampliar las pruebas a conjuntos de datos más grandes para observar cómo cada método escala y cómo las diferencias de rendimiento evolucionan con un aumento significativo en la cantidad de datos.

