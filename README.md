# User manual

## Installing dependencies

### Fedora

Run the following command

```cmd

sudo dnf install SDL2-devel

sudo dnf install ncurses-devel

```

### Ubuntu/Debian

Run the following command

```cmd

sudo apt-get install libsdl2-dev

sudo apt-get install libncurses5-dev libncursesw5-dev

```

## Compile and run the server

To compile the server you must run the following command

```cmd

gcc server.c -o server

```

To run the server you must run the following command

```cmd

./server <port>

```

## Compile and run the client

To compile the client you must run the following command

```cmd

gcc pong.c `sdl2-config --cflags --libs` -o pong

```

To run the client you must run the following command

```cmd

./client <ip> <port> <nickname>

```
