# BotStarCraft — Analítica de Eficiencia y Telemetría en StarCraft: Brood War

Proyecto desarrollado en **C++ utilizando BWAPI 4.4.0** (basado en el entorno *STARTcraft*) y **Python** para el análisis experimental de recolección de recursos, eficiencia espacial de minerales y telemetría de rendimiento de trabajadores en **StarCraft: Brood War**.

---

## Funcionalidades Implementadas

### 1. Monitorización y Eficiencia de Campos de Minerales (Pregunta 1)
* **Arquitectura Orientada a Objetos (`Mina.h` / `Mina.cpp`):** Cada cristal de mineral del mapa es modelado como un objeto independiente que registra su `ID`, coordenadas espaciales `(x, y)` y un historial deslizante de los **últimos 20 ciclos de extracción** (cada deducción de 8 unidades de recurso).
* **Asignación Óptima 1:1:** El bot distribuye automáticamente **9 trabajadores (1 trabajador por cristal)** evitando colisiones o filas de espera en un mismo mineral.
* **Visualización en Tiempo Real (`drawTextMap`):** Sobre cada cristal se renderizan tres métricas en vivo:
  * `Ult`: Frames transcurridos en la última extracción completada.
  * `Prom`: Promedio móvil de frames entre extracciones (a menor promedio, mayor eficiencia).
  * `Act`: Contador de frames del ciclo actual en curso.

### 2. Sistema de Telemetría de Rendimiento y Microgestión (Pregunta 2)
* **Auditoría del Ciclo de Trabajo por Unidad (`drawTextMap`):** Registra en cada fotograma el estado operativo de cada unidad recolectora, mostrando sobre ella la proporción exacta entre **Tiempo de Desplazamiento (`Viaje %`)** y **Tiempo de Extracción Efectiva (`Pico %`)**.
* **Panel Macroeconómico en Pantalla (`drawTextScreen`):** Muestra en tiempo real:
  * **Índice de Saturación de Base:** Relación trabajadores activos / minerales disponibles (`SUB-SATURADA`, `OPTIMA` o `SOBRE-SATURADA`).
  * **Flujo Real de Ingresos:** Total de minerales recolectados y tasa de extracción en **Minerales por Minuto (`M/min`)**.

### 3. Automatización Experimental y Análisis de Datos en Python
* **Ventana Estandarizada de 3 Minutos (`4320` frames a 24 FPS):** Al cumplirse exactamente 3 minutos de partida, el bot detecta automáticamente la raza (**Terran, Protoss o Zerg**) y el lado del mapa (**Izquierda o Derecha**), exporta los resultados a `resultados/resultados_<Raza>_<Lado>.csv` y pausa la partida (`pauseGame()`) para inspección visual.
* **Procesamiento y Visualización (`resultados/analizar_minas.py`):** Script en Python (`pandas` y `matplotlib`) que valida la presencia de las 6 muestras experimentales ($3\text{ razas} \times 2\text{ lados}$), identifica el **Top 4 de minerales más eficientes** por raza y formación, y genera tablas resumen (`.csv`) y gráficas comparativas (`.png`).

---

## Estructura del Proyecto

    BotStarcraft/
    ├── bin/                    # Ejecutables y scripts de lanzamiento en Windows
    ├── bin_linux/              # Scripts de lanzamiento mediante Wine en Linux
    ├── src/
    │   ├── bwapi/              # Cabeceras y librerías de BWAPI 4.4.0
    │   └── starterbot/         # Código fuente principal (StarterBot, Mina, Tools, MapTools)
    ├── resultados/             # Archivos CSV exportados por partida, gráficas y analizar_minas.py
    ├── starcraft/              # Instalación de StarCraft: Brood War 1.16.1
    ├── visualstudio/           # Solución STARTcraft.sln para Visual Studio 2022
    └── Makefile                # Compilación cruzada para Linux/macOS con Mingw-w64

---

## Ejecución del Análisis en Python

Una vez completadas las 6 partidas experimentales (3 minutos cada una) en el mapa `4Retro1.2_1.16-Backport.scx`:

1. Verifica que en la carpeta `resultados/` existan los 6 archivos generados automáticamente:
   * `resultados_Terran_Izquierda.csv` y `resultados_Terran_Derecha.csv`
   * `resultados_Protoss_Izquierda.csv` y `resultados_Protoss_Derecha.csv`
   * `resultados_Zerg_Izquierda.csv` y `resultados_Zerg_Derecha.csv`
2. Ejecuta el script de análisis:
   ```bash
   cd resultados
   python3 analizar_minas.py

## Ejecución del Análisis en Python

Una vez completadas las 6 partidas experimentales (3 minutos cada una) en el mapa `4Retro1.2_1.16-Backport.scx`:

1. Verifica que en la carpeta `resultados/` existan los 6 archivos generados automáticamente:
* `resultados_Terran_Izquierda.csv` y `resultados_Terran_Derecha.csv`
* `resultados_Protoss_Izquierda.csv` y `resultados_Protoss_Derecha.csv`
* `resultados_Zerg_Izquierda.csv` y `resultados_Zerg_Derecha.csv`


2. Ejecuta el script de análisis:
```bash
cd resultados
python3 analizar_minas.py

```


3. Se generarán automáticamente `tabla_top4_eficiencia.csv`, `resumen_comparativo_razas.csv` y las gráficas `grafica_top4_por_raza.png`, `grafica_comparacion_lados.png` y `grafica_mapa_espacial.png`.

---

## Instrucciones de Instalación y Compilación

### Windows / C++

Este repositorio incluye BWAPI 4.4.0 y utiliza *Injectory* para lanzar StarCraft con el bot sin necesidad de Chaoslauncher.

1. Clona o descarga este repositorio en tu computadora.
2. Descarga y descomprime [StarCraft: Brood War 1.16.1](https://www.cs.mun.ca/~dchurchill/starcraftaicomp/files/startcraft/scbw_bwapi440.zip) dentro de la carpeta `starcraft/`.
3. Ejecuta `bin/RunStarterBotAndStarcraft.bat` para iniciar el bot y StarCraft con BWAPI.
4. Abre `visualstudio/STARTcraft.sln` en **Visual Studio 2022** para modificar o recompilar el código.

### Linux / C++

1. Clona o descarga este repositorio en tu computadora.
2. Descarga y descomprime [StarCraft: Brood War 1.16.1](https://www.cs.mun.ca/~dchurchill/starcraftaicomp/files/startcraft/scbw_bwapi440.zip) dentro de la carpeta `starcraft/`.
3. Instala **Mingw-w64**:
```bash
sudo apt install build-essential mingw-w64

```


4. Instala **Wine**:
```bash
sudo dpkg --add-architecture i386 && sudo apt update && sudo apt install wine

```


*(Si utilizas [WINE_AppImage](https://github.com/mmtrt/WINE_AppImage), ajusta `WINEPREFIX` y la ruta de Wine dentro de `bin_linux/RunStarterBotAndStarcraft.sh`).*
5. Ejecuta `bash bin_linux/RunStarterBotAndStarcraft.sh` para lanzar el bot y el juego.
6. Recompila los cambios desde la raíz del proyecto ejecutando `make`.

> **Nota:** En la carpeta `bin_linux/`, las librerías `libgcc_s_dw2-1.dll` y `libstdc++-6.dll` corresponden a los binarios incluidos en `/usr/lib/gcc/i686-w64-mingw32/12-win32` tras instalar Mingw-w64.

---

## Recursos Base de STARTcraft

* Introducción a StarCraft AI: https://www.youtube.com/watch?v=czhNqUxmLks
* Tutorial de STARTcraft: https://www.youtube.com/watch?v=FEEkO6__GKw

```

```