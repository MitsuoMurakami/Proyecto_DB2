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

## Backend: Índice Invertido

### Construcción del Índice Invertido
- Construcción del índice invertido implementación propia (en Python).
  * Implementación en memoria secundaria y estructura de datos utilizada

- Ejecución óptima de consultas aplicando Similitud de Coseno
  * 

- Construcción del índice invertido en PostgreSQL:
   1. Primero, agregamos una columna full_text en la tabla Canciones para almacenar la información concatenada del nombre de la canción, el nombre del artista y la letra. Esta columna facilita el análisis y búsqueda textual sobre múltiples atributos.
   2. Creamos un atributo de tipo vectorial, en este caso nosotros convertimos la columna(full_text) en la que concatenamos el nombre de la canción, el nombre del artista y la letra de la canción. PostgreSQL optimiza las palabras de la columna para una búsqueda eficiente mediante procesamiento lingüístico (por ejemplo, eliminando palabras comunes y aplicando lematización).
   3. Luego Creamos el índice GIN sobre dicho vector. GIN está diseñado para manejar casos en los que los elementos que se van a indexar son valores compuestos y las consultas que debe manejar el índice deben buscar valores de elementos que aparecen dentro de los elementos compuestos. Por ejemplo, los elementos podrían ser documentos y las consultas podrían ser búsquedas de documentos que contengan palabras específicas.
Un índice GIN almacena un conjunto de pares (clave, lista de contabilización), donde una lista de contabilización es un conjunto de identificadores de fila en los que aparece la clave.
   4. Por último aplicamos las consultas con el índice y la query usando como score la similitud coseno retornándonos un top K, teniendo en cuenta que las palabras de la query tiene que estar separada por el símbolo de or (|).


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


## Backend: Índice Multidimensional

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
[Incluir tablas con métricas relevantes]

#### Gráficos Comparativos
[Incluir gráficos mostrando rendimiento y comparaciones]

### Análisis y Discusión
- Interpretación de resultados
- Conclusiones principales
- Recomendaciones futuras

