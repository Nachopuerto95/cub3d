<p align="end">
  <strong>🌐 Cambiar idioma:</strong><br>
  <a href="/README.es.md">
   <img src="https://github.com/Nachopuerto95/multilang/blob/main/ES.png" alt="Español" width="50">
 </a>&nbsp;&nbsp;&nbsp;
 <a href="/README.md">
   <img src="https://github.com/Nachopuerto95/multilang/blob/main/EN.png" alt="English" width="50">
 </a>
</p>

# Cub3D (42cursus)

<img src="https://github.com/Nachopuerto95/multilang/blob/main/42-Madrid%20-%20Edited.jpg">

## 📜 Acerca del Proyecto

```html
	🕹️ Cub3D es un proyecto donde aprenderás fundamentos de gráficos 3D mediante la creación de un motor gráfico simple
   basado en raycasting, inspirado en clásicos como Wolfenstein 3D.
```

> [!NOTE]  
> Debido a los requisitos de la norma de la Escuela 42:
> * Cada función no puede tener más de 25 líneas de código.
> * Todas las variables deben ser declaradas y alineadas en la parte superior de cada función.
> * El proyecto debe ser creado solo con las funciones permitidas.
<br>

### 📌 Descripción

Cub3D consiste en desarrollar un pequeño motor gráfico 3D utilizando la técnica de raycasting. El objetivo es renderizar un entorno laberíntico en primera persona, permitiendo al usuario explorarlo en tiempo real. Se trabaja con gráficos 2D y matemáticas para simular una perspectiva 3D, gestionando colisiones, texturas y movimiento del jugador.

Este repositorio contiene una implementación completa del proyecto Cub3D, desarrollada en lenguaje C, cumpliendo con las restricciones y normas establecidas por la escuela 42.

# 🚀 Guía para Cub3d 42.

Para explicar este proyecto y afianzar mis conocimientos sobre el mismo, he decidido hacer una especie de guía que otros alumnos de 42 puedan utilizar para apoyarse en las fases iniciales del proyecto. vamos a dar por sentado conocimientos tanto en el lenguaje C como
en el uso de la librería Minilibx que muy posiblemente hayamos tenido que utilizar en otros proyectos llegados a este punto del cursus.

Comenzaremos por explicar como funcionan los principales hooks de la librería necesarios para el juego, el movimiento del jugador y el algoritmo de ray casting (una versión simplificada).

### Main:

```c
  int main(void)
  {
      t_game game;
      init_game(&game);
  
      mlx_hook(game.win, 2, 1L<<0, key_press, &game.player);
      mlx_hook(game.win, 3, 1L<<1, key_release, &game.player);
      mlx_loop_hook(game.mlx, draw_loop, &game);
  
      mlx_loop(game.mlx);
      return 0;
  }
```
En la estructura game, vamos a añadir las variables necesarias para mlx (mlx, win...) añadiremos más cosas en el futuro pero vamos por orden.
tras declarar nuestra estructura y definir sus variables en el valor inicial, comenzamos con los hooks.

#### Hooks:

hooks de teclado:

```c
  mlx_hook(game.win, 2, 1L<<0, key_press, &game.player);
```
como primer argumento le pasamos la ventana, los argumentos segundo y tercero hacen referencia a los eventos key press y key release, déjalos así, después el nombre de la función que hemos creado para dicho evento
en mi caso, se llama `key_press()` y `key_release()` pero podrían llamarse `patata()` por último un puntero a la estructura donde haremos los cambios.
por ejemplo, si yo quiero que mientras W esté pulsado la posición Y del player incremente, necesito la estructura player para cambiar su posY`

hook para el loop:

Prácticamente todos los juegos necesitan un game loop (bucle de juego) para funcionar correctamente. Este loop es un ciclo que se ejecuta de forma continua mientras el juego está en marcha. Su función principal es realizar todos los cálculos necesarios en cada "frame", como:

- Procesar la entrada del usuario (teclado, ratón…)
- Actualizar la posición del jugador y los elementos del entorno
- Detectar colisiones
- Redibujar la pantalla con la nueva información

 De la misma manera que los demás hooks, vamos a pasarle una función, que será la que se repita continuamente.

vamos a dividir esta función en las 3 acciones principales que va a llevar a cabo: limpiar, recalcular y pintar.

de esta forma primero borraremos lo que pintamos en el anterior frame, recalcularemos lo que tenemos que pintar en el frame actual y lo pintaremos.


