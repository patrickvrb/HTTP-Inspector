#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 8228 /* Porta Proxy Local 8228*/

int main(int argc, char const *argv[])
{

    int server_socket;

        /* Criando o Socket */
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        printf("Socket Falhou!\n");
    }
    else printf("Socket Criado com Sucesso!\n");





    printf("Mensagem Recebida\n");
    printf("Mensagem Enviada\n");
    return 0;
}
