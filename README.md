# Pong

Para compilar el pong se debe correr el siguiente comando

```
gcc pong.c `sdl2-config --cflags --libs` -o pong
```

Para ejecutar el cliente se debe ubicar en la carpeta Cliente y escribir el siguiente comando

```
./pong
```

Se debe instalar lo siguiente (en fedora)

```
sudo dnf install SDL2-devel

sudo dnf install ncurses-devel
```
