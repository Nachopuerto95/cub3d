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
	🕹️ Cub3D es un proyecto donde aprenderás fundamentos de gráficos 3D mediante la creación de un
	motor gráfico simple basado en raycasting, inspirado en clásicos como Wolfenstein 3D.
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

<p align="center">
  <img src="https://github.com/Nachopuerto95/cub3d/blob/main/img/ezgif-88233df297ca3c.gif" alt="Captura de Cub3D" width="400"/>
</p>

Para que nuestro juego sea más jugable, nosotros manejamos con `WASD`la posición del jugador y con las flechas de dirección izquierda y derecha; hacia donde "mira" el jugador (movemos su angulo de visión).
para esto necesitamos introducir los conceptos de seno y coseno, que como yo no soy matemático intentaré explicar de la forma menos errónea posible.

nuestro player va a tener una nueva variable; `angle` esta define a groso modo, hacia donde mira el player

es común definir la dirección en la que mira el jugador usando un ángulo en radianes.
Normalmente, se toma el eje X positivo como referencia (mirar a la derecha), y a partir de ahí el ángulo aumenta en sentido antihorario:

Derecha: 0 radianes (o 2π, es lo mismo)
Arriba: π/2 radianes
Izquierda: π radianes
Abajo: 3π/2 radianes (o -π/2)


<p align="center">
  <img src="https://github.com/Nachopuerto95/cub3d/blob/main/img/pi-angles.png" alt="Captura de Cub3D" width="400"/>
</p>

Básicamente, iniciaremos la variable `angle`en una de estas direcciones, dependiendo de hacia donde queremos que mire el player al inicio de la partida.

```html
Guarda una constante de PI en tu .h con unos 12 decimales, lo vamos a utilizar bastante
```

También vamos a necesitar una constante llamada ANGLE_SPEED, básicamente va a ser la velocidad a la que gira nuestro player, yo la he definido como 0.01, vamos a sumarle esta cantidad a angle, cada vez que detectemos que `player->arrow_left` o `player->arrow_right` son true, también es importante restear sus valores al dar una "vuelta completa":

```c
    if (player->arrow_left)
        player->angle -= angle_speed;
    if (player->arrow_right)
        player->angle += angle_speed;
    if (player->angle > 2 * PI) // si angle supera a 2 * PI, lo ponemos a 0 y viceversa
        player->angle = 0;
    if (player->angle < 0)
        player->angle = 2 * PI;
```

El ángulo al que mira el player va a afectar directamente al movimiento del jugador con las teclas WASD, pues, ahora queremos que al pulsar la W, el player se mueva en la dirección que marca el ángulo de esta forma. es decir, ya no nos basta con sumar o restar PLAYER_SPEED a posX o posY, ahora hemos de tener en cuenta cual es la desviación tanto en X como en Y que provoca el ángulo. lo haremos de esta forma:

¿Qué son seno y coseno en este contexto?
Seno (sin) y coseno (cos) son funciones matemáticas que te dicen “cuánto te mueves en X” (horizontal) y “cuánto te mueves en Y” (vertical) cuando quieres avanzar en la dirección hacia donde está mirando el jugador.
Imagina un círculo. El ángulo (player->angle) es hacia dónde mira la flecha/jugador.
Coseno es el movimiento horizontal (X), seno es el movimiento vertical (Y).
¿Para qué sirven aquí?
Sirven para mover al jugador en la dirección que está mirando, no solo hacia arriba/abajo/izquierda/derecha fijos en la pantalla, sino hacia donde apunta la flecha.
Por ejemplo: Si miras arriba a la derecha, avanzarás en diagonal solo pulsando`W`.

para moverte hacia alante y atras, sumamos o restamos`Cos(angle)` para x y Sin(angle) para y, para el movimiento lateral se invierte y utilizamos Sin(angle) para x y Cos(angle) para y:

```c
    if (player->key_up)
    {
        player->x += cos_angle * PLAYER_SPEED;
        player->y += sin_angle * PLAYER_SPEED;
    }
    if (player->key_down)
    {
        player->x -= cos_angle * PLAYER_SPEED;
        player->y -= sin_angle * PLAYER_SPEED;
    }
    if (player->key_left)
    {
        player->x += sin_angle * PLAYER_SPEED;
        player->y -= cos_angle * PLAYER_SPEED;
    }
    if (player->key_right)
    {
        player->x -= sin_angle * PLAYER_SPEED;
        player->y += cos_angle * PLAYER_SPEED;
    }
```

¿Sencillo verdad?, ahora vamos a castear nuestro primer rayo en la dirección que tenemos en angle, para hacer mas evidente todo lo que hemos implementado hasta el momento.

```c
void draw_line(t_player *player, t_game *game, float angle, int i)
{
    float cos_angle = cos(angle);
    float sin_angle = sin(angle);
    float ray_x = player->x;
    float ray_y = player->y;

    while(!touch(ray_x, ray_y, game))
    {
        put_pixel(ray_x, ray_y, 0xFF0000, game);
        ray_x += cos_angle;
        ray_y += sin_angle;
    }
}
```
Dibujar la línea es parecido a movernos, vamos a ir pintando píxeles en las posiciones ray_x e y, que van a ir incrementando en la dirección dada por seno y coseno del angulo pero en vez de depender de si pulsamos una tecla, lo vamos a hacer hasta que `touch() nos devuelva true, básicamente esta funcion va a comprobare si en las posiciones x,y de nuestra matriz hay un `1`,

```c
bool touch(float px, float py, t_game *game)
{
    int x = px / BLOCK;
    int y = py / BLOCK;
    if(game->map[y][x] == '1')
        return true;
    return false;
}
```

presta atención a que hemos dividido px / BLOCK, ya que px hace referencia a la pantalla que puede ser en mi caso de 576x448, para tener la posición en la matriz, dividimos por el tamaño de nuestra celda (BLOCK) 576 / 64 = 9 y 448 / 64 = 7, es decir, nuestra matriz tendrá posiciones de 0 a 8 en horizontal y de 0 a 6 en vertical. a menudo vamos a necesitar convertir entre la x de la pantalla y la x de la matriz de esta forma.

Una vez implementado esto, nuestro programa debería de verse así, con nuestro primer rayo casteado. A  partir de aquí todo toma un poco más de forma y lo vas a entender muy bien.

<p align="center">
  <img src="https://github.com/Nachopuerto95/cub3d/blob/main/img/ezgif-76bd7f53bd990d.gif" alt="Captura de Cub3D" width="400"/>
</p>

El siguiente paso para entender nuestro algoritmo va a ser lanzar unos cuantos rayos más, para que la explicación sea más visual.
antes hemos lanzado un rayo en la dirección de angle, como lo que queremos es hacer un arco, vamos a introducir dos variables más y para hacer la prueba vamos a lanzar 10 rayos

```c
   	float fraction = PI / 3 / 10; // Esto define la distancia entre cada rayo
	float start_x = player->angle - PI / 6; // esto define el ángulo de visión.
```

voy a explicar un poco mejor lo del ángulo de visión por que así nos liamos un poco:

Copilot said: El ángulo de visión **π/3 radianes** equivale
El ángulo de visión π/3 radianes equivale a 60 grados.

Esto se debe a que para convertir de radianes a grados, multiplicas por 180 y divides entre π:

```html
(π / 3) × (180 / π) = 60°
```
como nosotros queremos un ángulo de 60º, es decir π / 3, nuestro arco va a comenzar desde angle - (π / 6) y va a ir hasta angle + (π / 6), de esta forma el total es π / 3 quedando al centro el "angle"
intento explicarlo en la siguiente imagen:

<p align="center">
  <img src="https://github.com/Nachopuerto95/cub3d/blob/main/img/FOV-img.png" alt="Captura de Cub3D" width="400"/>
</p>

ahora podemos entender algo mejor esta parte:

```c
   	float fraction = PI / 3 / 10;
	float start_x = player->angle - PI / 6;
```
en nuestra prueba con 10 rayos, vamos a sacar el fraction (separacion entre cada rayo) a partir del campo de vision total `PI / 3` dividido entre el número de rayos `10`
y start_x va a ser donde comienza nuestro primer rayo `PI / 6` como ya hemos explicado antes.

si nosotros en el loop, en lugar de pintar un rayo desde la posicion angle hacemos un bucle que pinte 10 rayos, desde `start_x` y en cada iteración a start_x le sumamos `fraction` podremos ver el arco del campo de visión. aquí va un ejemplo del bucle:

```c
    while(i < 10)
    {
        draw_line(player, game, start_x, i);
        start_x += fraction;
        i++;
    }
```
y aquí el resultado:

<p align="center">
  <img src="https://github.com/Nachopuerto95/cub3d/blob/main/img/fov-10-rays.gif" alt="Captura de Cub3D" width="400"/>
</p>

Ahora imagina que cogemos cada uno de estos rayos, calculas la distancia que hay hasta que choca con el muro, a partir de esta distancia vas a calcular la altura de esa "linea de pixeles" que corresponde al punto donde ha chocado el rayo, teniendo la altura, la pintamos en la pantalla alineada al centro, esto ya nos va a dar la sensación de estar viendo una imagen en 3D, como se que es más facil de entender viéndolo, muestro un ejemplo en el que tiramos 20 rayos. 

<p align="center">
  <img src="https://github.com/Nachopuerto95/cub3d/blob/main/img/20rays.gif" alt="Captura de Cub3D" width="400"/>
</p>

y si ahora lanzamos un rayo por cada píxel del ancho de nuestro juego (576px en mi caso) vamos a poder pintar la linea vertical correspondiente a cada uno, consiguiendo una imagen como esta:

<p align="center">
  <img src="https://github.com/Nachopuerto95/cub3d/blob/main/img/fullwidthrays.gif" alt="Captura de Cub3D" width="400"/>
</p>

### calcular distancia:

Como puedes observar en el gif de arriba, hay un error cuando comprobamos las colisiones pixel a pixel llamado corner bug, que no reconoce la colisión entre dos celdas pegadas diagonalmente. por este motivo y por eficiencia, vamos a utilizar un algoritmo llamado DDA.

este algoritmo utiliza la trigonometría para calcular en que pared choca nuestro rayo.

mientras lo explicamos utilizaremos las siguientes variables:
`side_dixt_x` y `side_dist_y`: la distancia entre el player y la próxima pared (del eje x o y ) con la que choca el rayo,
`delta_dist_x` y `delta_dist_y`: la distancia que recorre el rayo entre una línea de un eje y otra del mismo eje, esta es constante:

We can get deltaDistX and deltaDistY using the following formulas:

```c
deltaDistX = abs(1 / rayDirX)
deltaDistY = abs(1 / rayDirY)
```

This formula is derived from pythagoras theorem, which is originally:


```c
deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX))
deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY))
```
and then simplified to :
```c
deltaDistX = abs(|rayDir| / rayDirX)
deltaDistY = abs(|rayDir| / rayDirY)
```
However, we can use 1 instead of |rayDir|, because only the ratio between deltaDistX and deltaDistY matters for the DDA code that follows later below


