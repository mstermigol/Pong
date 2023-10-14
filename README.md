# Pong

Para compilar el pong se debe correr el siguiente comando desde la carpeta "Client"

```
gcc -I "../GameFiles/headers" -o pong pong.c  "../GameFiles/game/gamestate.c" "../GameFiles/game/log.c" "../GameFiles/game/session.c" "../Protocol/scc.c" "../GameFiles/game/draw.c" `sdl2-config --cflags --libs`
```

Para compilar el servidor se debe correr el siguiente comando desde la carpeta "Server"

```
gcc -I "../GameFiles/headers" -o server server.c  "../GameFiles/game/gamestate.c" "../GameFiles/game/log.c" "../GameFiles/game/session.c" "../Protocol/scc.c" -lpthread
```

Para ejecutar el cliente se debe ubicar en la carpeta Cliente y escribir el siguiente comando

```
./pong [IP] [Puerto] [Nombre]
```

Para ejecutar el cliente se debe ubicar en la carpeta Cliente y escribir el siguiente comando
```
./server [Puerto]
```

Se debe instalar lo siguiente (en fedora)

```
sudo dnf install SDL2-devel

sudo dnf install ncurses-devel
```
