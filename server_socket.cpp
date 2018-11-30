#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <resolv.h>
#include <string.h>
#include <cstdint>
#include <stdint.h>
#define PORT 8228         /* Porta Proxy Local 8228 */
#define BUFFERSIZE 65535

void *runSocket(void *vargp);

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int server_socket, new_socket, valread;
    char buffer[BUFFERSIZE];

    int client_fd;

    signal(SIGPIPE,SIG_IGN);

    /* Criando o Socket */
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        /* IPV4, TCP, IP = 0 */
        printf("Socket Falhou!\n");
    }
    else printf("Socket Criado com Sucesso!\n");

    address.sin_family = AF_INET;   /* Protocolo IPV4 */
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT); /* Porta padrão 8228 */
    //proxy_sd.sin_port = htons(atoi(proxy_port));

    /* Socket para a porta padrão */
    if (bind(server_socket, (struct sockaddr*)&address, sizeof(address))<0)
    {
        printf("Bind falhou!\n");
    }
    if (listen(server_socket, 4) < 0)
    {
        printf("Não foi possível escutar!\n");
        exit(1);
    }
    printf("Escutando...\n");

    listen(server_socket, SOMAXCONN);

    /*
    if ((new_socket = accept(server_socket, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
    {
        printf("Não foi possível extrair conexão p/ novo socket!");
        exit(1);
    }*/

    while(1)
     {
          //Aceitar conexões
          client_fd = accept(server_socket, (struct sockaddr*)NULL ,NULL);
          printf("CLIENTE ID:  %d ACEITADO!/n",client_fd);

          if(client_fd > 0)
          {
                //multithreading variables
               printf("Proxy conectado!\n");
               pthread_t tid;
               // pass client fd as a thread parameter
               pthread_create(&tid, NULL, runSocket, (void *)client_fd);
          }
     }
     close(client_fd);

    /*
    valread = read(new_socket, buffer, 1024);
    //send(new_socket , hello , strlen(hello) , 0 ); template SEND
    printf("Mensagem do servidor!\n");*/
    return 0;
}

void *runSocket(void *vargp)
 {
      int c_fd;
      c_fd = (intptr_t) vargp; // get client fd from arguments passed to the thread
      //c_fd = *((int*)(&vargp)); TESTE
      char buffer[65535];
      int bytes = 0;

      while(1)
      {
           //Dados do cliente
           memset(&buffer,'\0',sizeof(buffer));
           bytes = read(c_fd, buffer, sizeof(buffer));
           if(bytes <0)
           {
                //perror("read");
           }
           else if(bytes == 0)
           {
           }
           else
           {
                //Echo p/ debug:
                write(c_fd, buffer, sizeof(buffer));
                //printf("client fd is : %d\n",c_fd);
                //printf("From client:\n");
                fputs(buffer,stdout);
           }
             fflush(stdout);
      };

      return NULL;
 }
