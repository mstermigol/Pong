# SCC (Sosa-Calvin-Checho) Protocol

## Introduction

The SCC protocol is a protocol for communication between a client and a server. It is a simple protocol that allows the client to send a request to the server and the server to respond to that request. The protocol is based on the HTTP protocol, but it is much simpler and does not have the same functionality.

## Protocol

### Player

- Structure

  `Player <playerNumber>`

- Description

  The Player message is sent by the server to the client when the client connects to the server. It contains the player number of the client. The player number is either 1 or 2. The client can use this information to determine which paddle it controls.

- Method

  ```c
  char *SendPlayer(int player)
  ```

### Start

- Structure

  `Start`

- Description

  The Start message is sent by the server to the client when the game starts. The client can use this information to start the game.

- Method

  ```c
  char *SendStart(2)
  ```

### GameState

- Structure

  `GameState <ballX> <ballY> <ballDx> <ballDy> <paddle1Y> <paddle2Y> <score1> <score2>`

- Description

  The GameState is a message sent by the server to the client. It contains the current state of the game (the position of the ball, the position of the paddles, and the score). The client can use this information to render the game.

- Method

  ```c
  char *SendGameState(GameState game)
  ```

### Move

- Structure

  `Move <direction> <playerNumber>`

- Description

  The Move message is sent by the client to the server. It contains the direction that the client wants to move its paddle and the player number of the client. The direction is either up or down. The server can use this information to update the position of the paddle. (0 is for down, 1 is for up)

- Example

  `Move 0 1`

- Method

  ```c
  char *SendMove(int direction, int player)
  ```

### End

- Structure

  `End <playerNumber>`

- Description

  The End message is sent by the server to the client when the game ends. It contains the number of the player that won the game. The client can use this information to display the winner of the game.

- Method

  ```c
  char *SendEnd(int player)
  ```

## Name

- Structure

  `Name <nickname>`

- Description

  The Name message is sent by the client to the server. It contains the nickname of the client. The server can use this information to write the nickname of the client in the log file.

- Method

  ```c
  char *SendName(const char *name)
  ```

## Receive

- Description

  Is how the protocol is going to be received by the server and the client.

- Method

  ```c
  int Receive(char *buffer)
  {
    if (strncmp(buffer, "Player", 6) == 0) {
        return 1;
    }
    else if (strncmp(buffer, "Start", 5) == 0) {
        return 2;
    }
    else if (strncmp(buffer, "GameState", 9) == 0) {
        return 3;
    }
    else if (strncmp(buffer, "Move", 4) == 0) {
        return 4;
    }
    else if (strncmp(buffer, "End", 3) == 0) {
        return 5;
    }
    else if (strncmp(buffer, "Name", 4) == 0) {
        return 6;
    }
    return 0;
  }
  ```
