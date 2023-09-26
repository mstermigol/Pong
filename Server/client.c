#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc > 2)
    {
        // Definir variables
        char *ip, buf[100];
        int socketFileDescriptor, port, numberOfBytes;
        port = atoi(argv[2]);
        ip = argv[1];

        // Estructuras
        // Estructura del servidor
        struct sockaddr_in server;
        // Estructura del cliente
        struct hostent *he;

        if ((he = gethostbyname(ip)) == NULL)
        {
            printf("Error en el gethostbyname() \n");
            exit(-1);
        }

        if ((socketFileDescriptor = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("Error en socket() \n");
            exit(-1);
        }

        // Configuracion del servidor
        // Familia de direcciones
        server.sin_family = AF_INET; // Familia TCP/IP
        // Puerto
        server.sin_port = htons(port); // Puerto
        // Direccion IP
        server.sin_addr = *((struct in_addr *)he->h_addr);
        bzero(&(server.sin_zero), 8); // Funcion que rellena con 0's

        // Conectarse al servidor
        if (connect(socketFileDescriptor, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
        {
            printf("Error en connect() \n");
            exit(-1);
        }

        // Recibir datos del servidor
        if ((numberOfBytes = recv(socketFileDescriptor, buf, 100, 0)) == -1)
        {
            printf("Error en recv() \n");
            exit(-1);
        }

        buf[numberOfBytes] = '\0';
        printf("Mensaje del servidor: %s \n", buf);

        // Cerrar el socket
        close(socketFileDescriptor);
    } else {
        printf("No se ingreso el ip y puerto por parametro \n");
    }
}