#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

// Funcion principal
int main(int argc, char **argv)
{
    if (argc > 1)
    {
        // Definir variables
        int socketFileDescriptor, newSocketFileDescriptor, port, clientLength;
        port = atoi(argv[1]);

        // Estructuras
        // Estructura del servidor
        struct sockaddr_in server;
        // Estructura del cliente
        struct sockaddr_in client;

        // Configuracion del servidor
        // Familia de direcciones
        server.sin_family = AF_INET; // Familia TCP/IP
        // Puerto
        server.sin_port = htons(port); // Puerto
        // Direccion IP
        server.sin_addr.s_addr = INADDR_ANY; // Cualquier cliente puede conectarse
        bzero(&(server.sin_zero), 8);        // Funcion que rellena con 0's

        // Creacion del socket
        if ((socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            perror("Error al crear el socket");
            exit(-1);
        }

        // Avisar que se creo el socket
        if (bind(socketFileDescriptor, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            perror("Error en el bind() \n");
            exit(-1);
        }

        // Establecer el socket en modo escucha
        if (listen(socketFileDescriptor, 5) == -1)
        {
            perror("Error en el listen() \n");
            exit(-1);
        }

        // Aceptar conexiones
        while (1)
        {
            clientLength = sizeof(struct sockaddr_in);
            if ((newSocketFileDescriptor = accept(socketFileDescriptor, (struct sockaddr *)&client, &clientLength)) == -1)
            {
                perror("Error en el accept() \n");
                exit(-1);
            }

            send(newSocketFileDescriptor, "Bienvenido al servidor \n", 22, 0);

            close(newSocketFileDescriptor);
        }

        close(socketFileDescriptor);
    }
    else
    {
        printf("No se ingresó por el puesto del parametro \n");
    }

    return 0;
}
