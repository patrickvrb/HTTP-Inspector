#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#define PORT 8228 /* Porta Proxy Local 8228 */
#define BUFFERSIZE 1024

int main(int argc, char const *argv[])
{
    struct sockaddr_in address; /*Struct pra Internet Address */
    int server_socket, new_socket, valread;
    char buffer[BUFFERSIZE];

        /* Criando o Socket */
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    {
        /* IPV4, TCP, IP = 0 */
        printf("Socket Falhou!\n");
    }
    else printf("Socket Criado com Sucesso!\n");

    address.sin_family = AF_INET;   /* Protocolo IPV4 */
    address.sin_port = htons(PORT); /* Porta padrão 8228 */

    /* Socket para a porta padrão */
    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address))<0)
    {
        printf("Bind falhou!\n");
    }
    if (listen(server_socket, 4) < 0)
    {
        printf("Não foi possível escutar cliente!\n");
        exit(1);
    }
    printf("Escutando cliente...\n");

    if ((new_socket = accept(server_socket, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
    {
        printf("Não foi possível extrair conexão p/ novo socket!");
        exit(1);
    }

    valread = read(new_socket, buffer, 1024);
    //send(new_socket , hello , strlen(hello) , 0 ); template SEND
    printf("Mensagem Enviada\n");
    return 0;
}
