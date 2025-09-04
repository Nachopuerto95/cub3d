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

para la creación del canvas ya deberíamos tener algo parecido a esto:

```c
    game->win = mlx_new_window(game->mlx, WIDTH, HEIGHT, "Game");
    game->img = mlx_new_image(game->mlx, WIDTH, HEIGHT);
```

de modo que para "limpiar" la pantalla solo debemos hacer un bucle que comience desde `x = 0` e `y = 0` hasta x = WIDTH` e y = `HEIGHT` rellenando cada pixel con el color de fondo, para pintar cada píxle utilizaremos una función parecida a esta, y atentos por que aquí empezamos a ver algunos conceptos importantes:

primero de todo vamos a necesitar las siguientes variables en nuestra estructura game:

`*char data`;
Es un puntero al inicio del "array" de bytes que representa la imagen completa en memoria. Cada píxel que dibujes modifica este array.

`int bpp`;
"Bits per pixel" — número de bits que ocupa cada píxel (habitualmente 32 en MiniLibX, es decir, 4 bytes por píxel).

`int size_line`;
Número de bytes que ocupa una línea completa de la imagen. Esto puede ser un poco mayor que WIDTH * (bpp / 8) por alineamiento de memoria.

`int endian;
Indica el orden de los bytes en la memoria (little/big endian). Suele ser irrelevante si solo usamos colores básicos y MiniLibX en Linux.

vamos a "iniciar" el addr de nuestra estructura, yo lo hice en la funcion init game que prepara todas las variables antes de los hooks, seguido de una primera impresión de la imagen

```c
	game->data = mlx_get_data_addr(game->img, &game->bpp, &game->size_line, &game->endian);
	mlx_put_image_to_window(game->mlx, game->win, game->img, 0, 0);
```

¿Para qué sirve mlx_put_image...?

Bueno este es el concepto importante al que me refería, en cada frame nosotros vamos a calcular de que color ha de ser cada pixel de la pantalla y lo vamos a guardar en el array data, este array es como una lista de todos los píxeles

- Multiplicamos y * size_line → Saltamos todas las filas anteriores.
- Multiplicamos x * (bpp / 8) → Nos movemos dentro de la fila actual hasta la columna x.
- Sumamos ambos valores → Obtenemos la posición exacta dentro de data donde está ese píxel.

No hace falta que tengamos esto en mente todo el rato, vamos a hacer una función a la que, pasandole la posición x e y del píxel y su color, nos va a colcar dicho pixel en su posicion en el array:

```c
	void	ft_put_pixel_t(int x, int y, unsigned int color, t_game *game)
	{
		char	*dst;
	
		if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) // comprueba que el pixel esté dentro de la pantalla
			return ;
		dst = game->data + (y * game->size_line + x * (game->bpp / 8));
		*(unsigned int *) dst = color;
	}
```
### Pintando mapa

Para que la explicación del raycasting sea mas accesible a personas que nunca han oido sobre el tema, vamos a empezar mostrando una vista cenital del mapa,

yo para esto he creado una función simple para pintar un cuadrado pasandole la posicion, el tamaño y el color:

```c
	void draw_square(int x, int y, int size, int color, t_game *game)
	{
	    for(int i = 0; i < size; i++)
	        put_pixel(x + i, y, color, game);
	    for(int i = 0; i < size; i++)
	        put_pixel(x, y + i, color, game);
	    for(int i = 0; i < size; i++)
	        put_pixel(x + size, y + i, color, game);
	    for(int i = 0; i < size; i++)
	        put_pixel(x + i, y + size, color, game);
	}
```

Y ahora vas a elaborar una función, que lea la matriz de tu mapa y por cada `1`pinte un cuadrado de un color y  por cada `0` un cuadrado de otro color, yo he utilziado para los muros otra función de `draw_filled_square()` pero bueno, tampoco puedo dártelo todo hecho.

esta función que pinta el mapa, llamemosla "draw_map" la vamos a llamar en cada iteracion del loop principal, junto con otra que pinte a nuestro player.
la posición del jugador por ahora va a ser el centro de la pantalla así que en la estructura player, pondremos su posX y su posY en width / 2` y `height / 2` podemos simplemente utilizar la funcion draw_square en la posicion x e y del jugador

el loop (muy simplificado) ya debería de ser algo así:

```c
int draw_loop(t_game *game)
{
    t_player *player = &game->player;

    clear_image(game); // Limpiar

    draw_square(player->x, player->y, 10, 0x00FF00, game); // pintar player
    draw_map(game); // pintar mapa
    
    mlx_put_image_to_window(game->mlx, game->win, game->img, 0, 0);
    return 0;
}
```
vamos a iniciar nuestro juego, y después de quebrarte la cabeza arreglando muchas cosas que no he explicado o que posiblemente hayan salido mal (es el primer intento, no te preocupes) deberías de poder ver algo parecido a esto:

<p align="center">
  <img src="https://github.com/Nachopuerto95/cub3d/blob/main/img/Captura%20desde%202025-09-02%2008-50-09.png?raw=true" alt="Captura de Cub3D" width="400"/>
</p>

### movimiento del player

Para que ese cuadradito pueda moverse vamos a hacer un par de funciones muy simples que ya hemos mencionado antes, key_press()` y `key_release()`, las cuales hemos pasado a los hooks del teclado.
estas funciones no hacen otra cosa que cambiar una serie de booleans que vamos a incluir en nuestra estructura player, cada uno hará referencia a una tecla, de esta forma nuestra estructura guardará la información de que tecla esta presionada o no.

cada vez que pulsamos una tecla, se activará nuestro hook, este generará un "keycode" que hace referencia al código correspondiente a la tecla que se ha pulsado y se lo pasará a la funcion que le hemos especificado

```c
mlx_hook(game.win, 2, 1L<<0, key_press, &game.player); // aquí el hook
```

```c
int key_press(int keycode, t_player *player)
{
    if(keycode == W)
        player->key_up = true;
    if(keycode == S)
        player->key_down = true;
    if(keycode == A)
        player->key_left = true;
    if(keycode == D)
        player->key_right = true;
    if(keycode == LEFT)
        player->left_rotate = true;
    if(keycode == RIGHT)
        player->right_rotate = true;
    return 0;
}
```

las constantes `W` `A` `S` `D`... las he guardado en mi archivo .h para no poner ahí los keycode, que son poco intuitivos. en mi caso los keycode son los siguientes:

```c
# define W 119
# define A 97
# define S 115
# define D 100
# define LEFT 65361
# define RIGHT 65363
```

la función `key_release()` es igual, pero poniendo las variables a `false`

ahora podemos añadir a nuestro loop una funcion de `move_player()` que va a recalcular en cada frame la posición del jugador.
el movimiento hacia arriba abajo y los lados es muy sencillo, vamos a crear una constante que será PLAYER_SPEED y dentro de la función de movimiento vamos a decir que "mientras player->key_up == true", player->posX += PLAYER_SPEED.
hazlo así con todas las teclas que tienes guardadas como booleanos en tu estructura player, al incluir la función `move_player()` antes de las funciones de pintado, ya tendremos esa estructura "básica"; limpiar, calcular y pintar. después de investigar y solucionar posibles errores, ya deberías de tener algo como esto:




