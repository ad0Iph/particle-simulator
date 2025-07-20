# Particle simulator - Computación en GPU CC7515
- Integrantes: Adolfo Arenas P. - Alejandro Mori A.

---
Proyecto realizado para el curso Computación en GPU CC7515. El programa busca evidenciar a través de una representación gráfica de un sistema de partículas la diferencia de rendimiento que ofrece usar paralelismo con una GPU, en comparación a una implementación lineal en CPU. Por otro lado, el sistema de partículas diseñado muestra la interacción de N $\in [2, 6000]$ partículas con carga positiva o negativa en un sistema que se rige exclusivamente bajo la ley de Coulomb.
## Clonar y compilar
Para clonar el proyecto, desde una terminal nos localizamos en la carpeta que queramos, y ejecutamos el siguiente comando: 

```
git clone --recurse-submodules https://github.com/ad0Iph/particle-simulator.git
```

Luego, nos movemos dentro de la carpeta del proyecto con:
```
cd particle-simulator
```
Una vez dentro, compilamos el proyecto con:
```
cmake -S . -B build && cmake --build build
```
y lo ejecutamos con:
```
./build/ParticleSystem
```


## Interfaz

Dentro de la simulación se pueden controlar ciertos aspectos como:

1. `Charge magnitude`: Magnitud de la carga que emite cada partícula.
2. `Radius`: Radio que se muestra en la simulación. Cabe destacar que es solo visual y no influye en la interacción de partículas ya que por simplicidad se decidió que fueran solo puntuales.
3. `Number of particles`: El número N $\in [2, 6000]$ de partículas que se muestran en la simulación.
4. `Positive charge ratio`: Proporción de partículas positivas y negativas dentro de la simulación
5. `Use GPU`: Casilla que permite utilizar la implementación del algoritmo en GPU en vez de en CPU.

## Controles

El programa tiene dos teclas con las cuales se puede interactuar:

1. `Tecla G`: Muestra, para una partícula aleatoria, sus datos asociados, su trayectoria y los vectores normalizados de velocidad y aceleración. 
2. `Tecla R`: Desmarca la partícula seleccionada.
