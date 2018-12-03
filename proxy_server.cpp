#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <resolv.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 8228 /* Porta Proxy Local 8228 */
#define BUFFERSIZE 4096
#define BUFFEROP 200000000

char *server_response(char[]);
int get_ip(char *, char *);
using namespace std;

int main(int argc, char const *argv[])
{
     int browser_socket;

     /* Criando o Socket */
     if ((browser_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
     {
          /* IPV4, TCP, IP = 0 */
          printf("Socket Falhou!\n");
     }
     else
          printf("Socket Criado com Sucesso!\n");

     struct sockaddr_in browser_addr;
     browser_addr.sin_family = AF_INET;
     browser_addr.sin_addr.s_addr = INADDR_ANY;
     browser_addr.sin_port = htons(PORT);

     /* Bind */
     if (bind(browser_socket, (struct sockaddr *)&browser_addr, sizeof(browser_addr)) < 0)
     {
          printf("Bind falhou!\n");
          exit(-1);
     }

     if (listen(browser_socket, SOMAXCONN) < 0)
     {
          printf("Não foi possível escutar!\n");
          exit(1);
     }
     printf("Escutando...\n");

     int browser_conn, message;
     char request[BUFFERSIZE], *response;

     response = (char *)malloc(BUFFEROP);

     while (1)
     {
          browser_conn = accept(browser_socket, (struct sockaddr *)NULL, NULL);

          if (browser_conn <= 0)
          {
               printf("Vish, deu ruim ao aceitar \n");
               exit(-1);
          }
          else
          {
               printf("Conexao aceita! \n");
               //Receber data do Browser
               memset(&request, '\0', sizeof(request));
               message = recv(browser_conn, request, sizeof(request), 0);
               if (message < 0)
               {
                    perror("Leitura");
               }
               else
               {
                    printf("Pedido do Browser: %s\n", request);
                    strcpy(response, server_response(request));
                    printf("Resposta do server: %s", response);
                    free(response);
               }
               fflush(stdout);
          }
     }
     close(browser_conn);

     return 0;
}

char *server_response(char *request)
{
     char *hostname, *server_ip, c;
     struct hostent *host_entry;
     char newrequest[300];

     int i = 1, j = 1;

     c = request[0];
     hostname = (char *)malloc(50 * sizeof(char));
     server_ip = (char *)malloc(33 * sizeof(char));

     /* Achar o hostname a partir do request */
     while (c != '/')
     {
          c = request[i++];
     }
     i++;
     hostname[0] = request[i++];
     c = request[i];
     while (c != ' ')
     {
          hostname[j++] = request[i++];
          c = request[i];
     }
     hostname[j++] = '\0';
     strtok(hostname, "/");
     printf("Hostname: %s\n", hostname);

     get_ip(hostname, server_ip);

     // To convert an Internet network address into ASCII string
     // server_ip = inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0]));

     printf("IP Address: %s\n", server_ip);

     int proxy_server_socket, lak = 1;

     if ((proxy_server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
     {
          /* IPV4, TCP, IP = 0 */
          printf("Socket do Servidor Falhou!\n");
     }
     else
     printf("Socket do Servidor Criado com Sucesso!\n");

     setsockopt(proxy_server_socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&lak, sizeof(int));

     struct sockaddr_in proxy_as_client_address;
     struct hostent *ref;
     ref = gethostbyname(hostname);
     bcopy(ref->h_addr, &proxy_as_client_address.sin_addr, ref->h_length);
     proxy_as_client_address.sin_family = AF_INET;
     proxy_as_client_address.sin_port = htons(80);

     if (connect(proxy_server_socket, (struct sockaddr *)&proxy_as_client_address, sizeof(struct sockaddr_in)) == -1)
     {
          perror("Connection Error:");
          exit(1);
     }

     printf("CONECTOU PORRA !!!!\n");

     char *response;
     response = (char*)malloc(sizeof(BUFFEROP));

     //BOTANDO O REQUEST NO FORMATO PADRÃO 
     
     strcat(newrequest, "GET http://");
     strcat(newrequest, hostname);
     strcat(newrequest, "/ HTTP/1.1\r\n\r\n");

     write(proxy_server_socket, newrequest, sizeof(newrequest));
     read(proxy_server_socket, response, BUFFEROP);
     //send(proxy_server_socket, request, sizeof(request), 0);
     //recv(proxy_server_socket, &response, sizeof(response), 0);

     printf("PASSOU RECEIV !!!!\n");

     //close(proxy_server_socket);

     return response;
}

int get_ip(char *hostname, char *ip)
{
     struct hostent *he;
     struct in_addr **addr_list;
     int i;
     if ((he = gethostbyname(hostname)) == NULL)
     {
          herror("gethostbyname");
          return 1;
     }
     addr_list = (struct in_addr **)he->h_addr_list;
     for (i = 0; addr_list[i] != NULL; i++)
     {
          strcpy(ip, inet_ntoa(*addr_list[i]));
          return 0;
     }
     return 1;
}

void cache(char *request)
{
     FILE *cache;
}
