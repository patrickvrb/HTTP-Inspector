#include <stdio.h>
#include <stdlib.h>

#define PORT 8228

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    struct sockaddr_in serv_addr;

    int sock = 0, valread;
    char const *hello = "O cliente diz oi!";
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Criacao do socket falhou \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

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
    send(sock , hello , strlen(hello) , 0 );
    printf("Mensagem enviada!\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );
    return 0;
}
