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
#define BUFFEROP 4096

char *server_response(char[]);
int get_ip(char *, char *);
char *load_cache(char *r, int );
int save_cache(char *, int);
int fileSearch(char *, FILE *);
void sendBrowser(char *);
int findMethod(char *str);

using namespace std;

int cache_cont = 1;

int main(int argc, char const *argv[])
{
     int browser_socket, porta = 0;

     printf("\n*******   HTTP INSPECTOR   *******\n");
     printf("By: Danillo & Patrick\n");
     printf("Teleinformática e Redes 2 - UnB\n");
     printf("Deseja se conectar em qual porta? (0 - Padrão)   ");
     scanf("%d", &porta);
     if(porta == 0){
          printf("Porta padrao 8228 selecionada!\n");
          porta = PORT;
     }
     else 
     {
          printf("Porta %d selecionada!\n", porta);
          printf("Não se esqueça de configurar o Proxy no Browser!\n");
     }
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
     browser_addr.sin_port = htons(porta);

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

     int browser_conn, message;
     char request[BUFFERSIZE], *response;

     response = (char *)malloc(BUFFEROP*sizeof(char));

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
               //Receber Request do Browser
               memset(&request, '\0', sizeof(request));
               message = recv(browser_conn, request, sizeof(request), 0);
               if (message < 0)
               {
                    perror("Leitura");
               } else {
                    printf("Pedido do Browser:\n%s\n", request);
                    strcpy(response, server_response(request));
                    //response = server_response(request);
                    printf("Resposta do Servidor:\n%s\n", response);
                    if (save_cache(response, 0) != 0 )
                    {
                         printf("Erro de cache\n");
                         exit(-1);
                    } 
                    message = send(browser_conn, response, sizeof(response), 0);
                    if (message < 0)
                    {
                         perror("Escrita");
                    }
                    //sendBrowser(response);
                    printf("\nEscutando...\n");
               }
               fflush(stdin);
               fflush(stdout);
          }
     }
     //free(response);
     close(browser_conn);

     return 0;
}

void sendBrowser(char *str){

     char *http_data, c = str[0];
     http_data = (char *) malloc(BUFFERSIZE*sizeof(char));

     int i = 1, j = 2;

     while (c != '<')
     {
          c = str[i++];
          //printf("Char: %c\n", c);
     }
     http_data[0] = c;
     c = str[i++];
     http_data[1] = c;
     while (c != '')
     {
          c = str[i++];
          http_data[j++] = c;
         
     }
     http_data[j++] = '\0';

     char http_header[BUFFERSIZE] = "HTTP/1.1 400 Bad Request\r\n\n";
     strcat(http_header, http_data);


     int proxy_socket, sockopt = 1;

     if ((proxy_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
     {
          /* IPV4, TCP, IP = 0 */
          printf("Socket do Browser sFalhou!\n");
     }
     else
          printf("Socket do Browser Criado com Sucesso!\n");

     setsockopt(proxy_socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&sockopt, sizeof(int));

     struct sockaddr_in browser_address;
     browser_address.sin_family = AF_INET;
     browser_address.sin_port = htons(5000);
     browser_address.sin_addr.s_addr = INADDR_ANY;

     if (connect(proxy_socket, (struct sockaddr *)&browser_address, sizeof(struct sockaddr_in)) == -1)
     {
          perror("Erro ao conectar com o Browser:");
          exit(1);
     }
     printf("Browser conectado\n\n");

     int browser_conn, message;

     message = write(proxy_socket, http_header, sizeof(http_header));
     if (message < 0)
     {
          perror("Erro no envio ao Browser");
     } else {
          printf("Pedido Encaminhado ao Browser\n");
     }     
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


     struct sockaddr_in proxy_as_client_address;
     struct hostent *ref;
     ref = gethostbyname(hostname);
     bcopy(ref->h_addr, &proxy_as_client_address.sin_addr, ref->h_length);
     proxy_as_client_address.sin_family = AF_INET;
     proxy_as_client_address.sin_port = htons(80);
     setsockopt(proxy_server_socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&lak, sizeof(int));

     if (connect(proxy_server_socket, (struct sockaddr *)&proxy_as_client_address, sizeof(struct sockaddr_in)) == -1)
     {
          perror("Connection Error:");
          exit(1);
     }
     printf("Servidor conectado\n\n");

     char *response;
     response = (char*)malloc(sizeof(BUFFEROP));

     //BOTANDO O REQUEST NO FORMATO PADRÃO 
     
     switch(findMethod(request)){
          case 0:
               strcat(newrequest, "GET http://");
               break;
          case 1:
               strcat(newrequest, "POST http://");
               break;
          case -1:
               perror("Erro! Nem GET nem POST!\n");
               exit(-1);
               break;
     }
     
     strcat(newrequest, hostname);
     strcat(newrequest, "/ HTTP/1.1\r\n\r\n");
     printf("PROXY REQUEST: %s\n", newrequest);

     write(proxy_server_socket, newrequest, sizeof(newrequest));
     read(proxy_server_socket, response, BUFFEROP);
     //send(proxy_server_socket, request, sizeof(request), 0);
     //recv(proxy_server_socket, &response, sizeof(response), 0);

     close(proxy_server_socket);

     //load_cache(response, 0);

     return response;
}

int findMethod(char *str){
     if(str[0] == 'P'){
          return 1;
     } else if(str[0] == 'G') {
          return 0;
     } else {
          return -1;
     }
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

int save_cache(char *str, int mode)
{
     FILE *cache;

     cache = fopen("cache.txt","a+");
     if(cache == NULL){
          printf("Erro ao abrir o arquivo\n");
          return -1;
     }

     switch(mode){
          case 0:
               fprintf(cache, "Segmento No. %d\n", cache_cont++);
               fprintf(cache, "%s", str);
               fprintf(cache, "\n\n");

               return 0;
               break;
          default:
               printf("Modo cache inexistente\n");
               return -1;
               break;
     }

     fclose(cache);
}

char *load_cache(char *str, int mode)
{
     FILE *cache;

     cache = fopen("cache.txt","a+");
     if(cache == NULL){
          printf("Erro ao abrir o arquivo\n");
          exit(-1);
     }

     char *http_data, c = str[0];
     http_data = (char *) malloc(BUFFERSIZE*sizeof(char));

     int i = 1, j = 2, aux;

     while (c != '<')
     {
          c = str[i++];
          //printf("Char: %c\n", c);
     }
     http_data[0] = c;
     c = str[i++];
     http_data[1] = c;
     while (c != '')
     {
          c = str[i++];
          http_data[j++] = c;
         
     }
     http_data[j++] = '\0';
     //strtok(hostname, "/");

     switch(mode){
          case 0:
               if(fileSearch(http_data, cache) == 0)
               {
                    return http_data;
                    break;
               } else {
                    printf("Nada no cache\n");
                    char *nothing;
                    return nothing;
               }
          default:
               printf("Modo cache inexistente\n");
               char *nothing;
               return nothing;
               break;
     }

     fclose(cache);
}

int fileSearch(char *str, FILE *fp) {

	char str_c, file_c;
     int str_len, i = 1, flag = 1;

     str_len = strlen(str);
     //printf("Tamanho: %d\n", str_len);

     printf("String: %s\n", str);
     
     while( !feof(fp) ) {
          fread(&file_c,1, 1, fp);
          str_c = str[0];
          while(str_c != file_c)
          {
               fread(&file_c, 1, 1, fp);
          }

          printf("Iguais: %c e %c\n", str_c, file_c);

          while(flag == 1)
          {
               fread(&file_c, 1, 1, fp);
               str_c = str[i++];
               printf("String: %c e File: %c\n", str_c, file_c);
               if(str_c != file_c){
                    flag = 0;
                    break;
               } 
               else
               {
                    if(i == str_len) return 0;
               }
          }
          break;
     }
     return -1;
}
