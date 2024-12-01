# Proyecto BD2 - Búsqueda y Recuperación de la Información

## Tabla de Contenidos
- [Introducción](#introducción)
- [Backend: Índice Invertido](#backend-índice-invertido)
- [Backend: Índice Multidimensional](#backend-índice-multidimensional)
- [Frontend](#frontend)
- [Experimentación](#experimentación)

## Introducción
### Objetivo del Proyecto
El objetivo de nuestro proyecto es usar modelos de recuperación y búsqueda con la técnica del índice invertido como método de indexación en donde el usuario escribirá 1 o más palabras y en base a eso se retornara las canciones que tengan similitud entre lo que el usuario escribio y una estructura multidimensional que soporte en nuestro caso los audios de las canciones de nuestra data en donde se extraeran las características de estos para hacer búsquedas por similitud basado en los atributos musicales de las canciones teniendo una búsqueda precisa y rápida.

Luego, para una mejor experiencia para el usuario implementaremos una interfaz gráfica en donde el usuario ingresará las palabras o características de una canción y se devolvera las canciones más similares a lo que se ingresó.

### Dominio de Datos e Importancia de la Indexación
**Datos básicos de la canción:** id de la canción, nombre, artista, letras, popularidad y fecha de lanzamiento.

**Información del álbum:** nombre, id del álbum y su fecha de lanzamiento.

**Detalles de la playlist:** cada canción puede estar en una o varias playlists, cada una con un id, nombre, género y subgénero

**Atributos de audio:** como la bailabilidad, energía, tono, modo (mayor o menor), y el grado de vocalización del cantante.

**Características compositivas:** la acústica, los instrumentos utilizados, la vivacidad de la canción  y la positividad que se transmite.

**Duracion y lenguaje:** Tambien tenemos la duración de la canción y el idioma en que se canta


-**Importancia de la indexacion:** La indexación nos ayuda a que accedamos más rápidamente a nuestros datos ya que señalan la ubicación exacta de nuestras canciones en vez de buscar por toda nuestra base de datos, gracias a esto se reduce el uso de recursos de nuestros sistema, haciendo que nuestro código pueda ser escalable, también haciendo que las consultas del usuario sean bastante rápidas aunque nuestra data sea muy grande, finalmente esto nos ayuda en el ordenamiento, filtrado y similitud ya que al centrarse en características especificas como nombre o artista, la recuperación de la información se vuelve más flexible y rápida.

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

- Ejecución óptima de consultas aplicando Similitud de Coseno
  * 


### Construcción del índice invertido en PostgreSQL:
   1. Primero, agregamos una columna full_text en la tabla Canciones para almacenar la información concatenada del nombre de la canción, el nombre del artista y la letra. Esta columna facilita el análisis y búsqueda textual sobre múltiples atributos.
   2. Creamos un atributo de tipo vectorial, en este caso nosotros convertimos la columna(full_text) en la que concatenamos el nombre de la canción, el nombre del artista y la letra de la canción. PostgreSQL optimiza las palabras de la columna para una búsqueda eficiente mediante procesamiento lingüístico (por ejemplo, eliminando palabras comunes y aplicando lematización).
   3. Luego Creamos el índice GIN sobre dicho vector. GIN está diseñado para manejar casos en los que los elementos que se van a indexar son valores compuestos y las consultas que debe manejar el índice deben buscar valores de elementos que aparecen dentro de los elementos compuestos. Por ejemplo, los elementos podrían ser documentos y las consultas podrían ser búsquedas de documentos que contengan palabras específicas.
Un índice GIN almacena un conjunto de pares (clave, lista de contabilización), donde una lista de contabilización es un conjunto de identificadores de fila en los que aparece la clave.
   4. Por último aplicamos las consultas con el índice y la query usando como score la similitud coseno retornándonos un top K, teniendo en cuenta que las palabras de la query tiene que estar separada por el símbolo de or (|).

## Backend: Índice Multidimensional (Parte 2)

### Técnicas de Indexación
- Descripción de las librerías utilizadas

   Faiss es un repositorio de Facebook el cual contempla varios tipos de indexación. 

- Justificación de la elección

   Se utilizó IndexLSH debido a la alta dimensionalidad de la data. Este índice utiliza un método de cálculo de distancia de bajo costo, Hamming distance. Además de utilizar vectores binarios.

- Estructura de datos implementada

### Búsquedas
#### KNN Search
- Implementación
- Complejidad computacional
- Optimizaciones realizadas

#### Range Search
- Implementación (si aplica)
- Limitaciones y ventajas

### Maldición de la Dimensionalidad
- Análisis del problema

   * El Rtree no es eficiente por el alto solapamiento que ocurre al aplicar este índice.

- Estrategias de mitigación implementadas

   * Se implementó el índice 

- Resultados obtenidos

   * 

## Frontend

### Interfaz Gráfica
- Tecnologías utilizadas
- Arquitectura de la GUI
- Principales características

### Manual de Usuario
1. Instalación
2. Configuración
3. Uso básico
4. Funcionalidades avanzadas

### Capturas de Pantalla
[Incluir capturas de pantalla relevantes con descripciones]

### Análisis Comparativo ??????
- Comparación con implementaciones similares
- Ventajas y desventajas
- Características únicas

## Experimentación

### Resultados Experimentales
#### Tablas de Resultados
Tabla sobre la medicion del performance entre SPIMI (implementacion propia) y PostgreSQL
|    N     | SPIMI   | Postgres |
|----------|---------|----------|
| 1000     | 0.5931  | 0.0097   |
| 2000     | 0.5718  | 0.0152   |
| 4000     | 0.5843  | 0.0558   |
| 6000     | 0.5805  | 0.035    |
| 8000     | 0.5631  | 0.0456   |
| 10000    | 0.6636  | 0.054    |
| 12000    | 0.5873  | 0.0637   |
| 14000    | 0.5862  | 0.0739   |
| 16000    | 0.5829  | 0.0861   |
| 18000    | 0.6367  | 0.0983   |


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
![image](https://github.com/user-attachments/assets/66f8eed6-7f25-48a9-9222-7a07cbdf6066)

![image](https://github.com/user-attachments/assets/b3c6fdc3-1509-4408-8992-ee29cb90cdc3)


### Análisis y Discusión
- Interpretación de resultados
- Conclusiones principales
- Recomendaciones futuras

