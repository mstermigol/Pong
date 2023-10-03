# SCC (Sosa-Calvin-Checho) Protocol

## Introduction

The SCC protocol is a protocol for communication between a client and a server. It is a simple protocol that allows the client to send a request to the server and the server to respond to that request. The protocol is based on the HTTP protocol, but it is much simpler and does not have the same functionality.

## Protocol

### GameState

- Structure

    `GameState <ballX> <ballY> <ballDx> <ballDy> <paddle1Y> <paddle2Y> <score1> <score2>`

- Description

    The GameState is a message sent by the server to the client. It contains the current state of the game (the position of the ball, the position of the paddles, and the score). The client can use this information to render the game.

### Player

- Structure

    `Player <playerNumber>`

- Description
  
  The Player message is sent by the server to the client when the client connects to the server. It contains the player number of the client. The player number is either 1 or 2. The client can use this information to determine which paddle it controls.

### Move

- Structure

    `Move <direction> <playerNumber>`

- Description

    The Move message is sent by the client to the server. It contains the direction that the client wants to move its paddle and the player number of the client. The direction is either up or down. The server can use this information to update the position of the paddle. (0 is for down, 1 is for up)

- Example

    `Move 0 1`

