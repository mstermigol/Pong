# User manual

## Installing dependencies

### Fedora

Run the following command

```cmd

sudo dnf install SDL2-devel

```

### Ubuntu/Debian

Run the following command

```cmd

sudo apt-get install libsdl2-dev


```

## Compile and run the server

To compile the server you must run the following command

```cmd

gcc -I "../GameFiles/headers" -o server server.c  "../GameFiles/game/gamestate.c" "../GameFiles/game/log.c" "../GameFiles/game/session.c" "../Protocol/scc.c" -lpthread

```

To run the server you must run the following command

```cmd

./server <port> <Log file>

```

## Compile and run the client

To compile the client you must run the following command

```cmd

gcc -I "../GameFiles/headers" -o pong pong.c  "../GameFiles/game/gamestate.c" "../GameFiles/game/log.c" "../GameFiles/game/session.c" "../Protocol/scc.c" "../GameFiles/game/draw.c" `sdl2-config --cflags --libs`

```

To execute the client you must run the following command

```cmd

./client <ip> <port> <nickname>

```

## Documentation

- [SCC Protocol](Documentation/SCCProtocol.md)
- [Diagrams](Documentation/Diagrams.md)
- [Programming Rules](Documentation/ProgrammingRules.md)

## How to play

The left paddle is controlled by the first player and the right paddle is controlled by the second player.

### Controls

- **Up Key** - Move up
- **Down Key** - Move down

### Goal

The goal of the game is to score 5 points before your opponent does.

## Credits

### Authors

- **Sergio Andrés Córdoba** - [sergiocordobam](https://github.com/sergiocordobam)
- **Miguel Jaramillo** - [mstermigol](https://github.com/mstermigol)
- **Miguel Sosa** - [msosav](https://github.com/msosav)

### Acknowledgments

We used (and modified a little bit) the pong game in the following repo [flightcrank/pong](https://github.com/flightcrank/pong). We really appreciate the work of the author of this proyect :)
