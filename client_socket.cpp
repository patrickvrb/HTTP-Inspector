#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#define PORT 5000 /* Porta Proxy Local 5000 */

#define BUFFERSIZE 4096

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;

    int sock = 0, valread, cont = 1;
    char const *hello = "O cliente diz oi!";

    char request[BUFFERSIZE], response[BUFFERSIZE], buffer[BUFFERSIZE];

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Criacao do socket falhou \n");
        return -1;
    }

    //memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    serv_addr.sin_addr.s_addr = INADDR_ANY;


    /* Converter enderecos IPv4 and IPv6 para binario */
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nEndereco invalido/ nao suportado \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConexao falhou \n");
        return -1;
    }

    while(1)
    {
      printf("\nMensagem %d", cont++);
      printf("\n*****************************************************");
      printf("\nDigite aqui: ");
      fgets(request, sizeof(request), stdin);
      printf("Request: %s \n", request);
      //send(sock, request, sizeof(request), 0);
      write(sock, buffer, sizeof(buffer));
      //fflush(stdout);
      printf("\nResposta do Servidor: ");
      recv(sock, &response, sizeof(response), 0);
      printf("Resposta do servidor: %s \n", response);
      //read(sock, buffer, sizeof(buffer));
      //fputs(buffer, stdout);
      //printf("%s\n", buffer);
      printf("*****************************************************\n");
      fflush(stdout);
    }
      close(sock);

    /*
    send(sock , hello , strlen(hello) , 0 );
    printf("Mensagem do cliente!\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );*/
    return 0;
}
