# compilación
- `cmake -B build`
- `cmake --build build`
- `./build/ParticleSystemApp`

# Modificar ejecución
- Cambiar secuencial/paralelo:
    - existen dos formas, solo hay que comentar/descomentar los bloques que empiezan desde:
        - linea 94 para la versión paralela
        - Línea 114 para la versión secuencial
- Cambiar parámetros:
    - línea 15: `NUM_PARTICLES` es el número de partículas totales
    - línea 33: `radius` es el tamaño de los círculos/partículas
    - línea 92: `k` es la fuerza de atracción/repulsión

# Configuraciones interesantes
- Mostrar interacción:
    - `NUM_PARTICLES = 10`
    - `radius = 0.03f`
    - `k = 0.1f`
- Mostrar diferencia de desempeño:
    - `NUM_PARTICLES = 2000`
    - `radius = 0.01f`
    - `k = 0.1f`