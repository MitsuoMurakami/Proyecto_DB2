# Proyecto BD2 - Búsqueda y Recuperación de la Información

## Tabla de Contenidos
- [Introducción](#introducción)
- [Backend: Índice Invertido](#backend-índice-invertido)
- [Backend: Índice Multidimensional](#backend-índice-multidimensional)
- [Frontend](#frontend)
- [Experimentación](#experimentación)

## Introducción
### Objetivo del Proyecto
[Aquí va la descripción del objetivo principal del proyecto]

### Dominio de Datos e Importancia de la Indexación
[Explicar el dominio de datos específico y por qué es crucial la indexación en este contexto]

## Backend: Índice Invertido

### Construcción del Índice Invertido
- Construcción del índice invertido implementación propia (en Python).
  * Implementación en memoria secundaria y estructura de datos utilizada

- Ejecución óptima de consultas aplicando Similitud de Coseno
- Construcción del índice invertido en PostgreSQL:
   1. Primero, agregamos una columna full_text en la tabla Canciones para almacenar la información concatenada del nombre de la canción, el nombre del artista y la letra. Esta columna facilita el análisis y búsqueda textual sobre múltiples atributos.
   2. Creamos un atributo de tipo vectorial, en este caso nosotros convertimos la columna(full_text) en la que concatenamos el nombre de la canción, el nombre del artista y la letra de la canción. PostgreSQL optimiza las palabras de la columna para una búsqueda eficiente mediante procesamiento lingüístico (por ejemplo, eliminando palabras comunes y aplicando lematización).
   3. Luego Creamos el índice GIN sobre dicho vector. GIN está diseñado para manejar casos en los que los elementos que se van a indexar son valores compuestos y las consultas que debe manejar el índice deben buscar valores de elementos que aparecen dentro de los elementos compuestos. Por ejemplo, los elementos podrían ser documentos y las consultas podrían ser búsquedas de documentos que contengan palabras específicas.
Un índice GIN almacena un conjunto de pares (clave, lista de contabilización), donde una lista de contabilización es un conjunto de identificadores de fila en los que aparece la clave.
   4. Por último aplicamos las consultas con el índice y la query usando como score la similitud coseno retornándonos un top K, teniendo en cuenta que las palabras de la query tiene que estar separada por el símbolo de or(|) 



## Backend: Índice Multidimensional

### Técnicas de Indexación
- Descripción de las librerías utilizadas
- Justificación de la elección
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
- Estrategias de mitigación implementadas
- Resultados obtenidos

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

