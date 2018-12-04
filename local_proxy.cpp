#include <sys/socket.h>
#include <sys/types.h>
#include <resolv.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

#define PORT 8228 /* Porta Proxy Local 8228 */

#define BUFFERSIZE 4096


 int main(int argc,char *argv[])
 {
     pthread_t tid;
     char port[100];
     char ip[100] = "172.217.162.174";
     char const *hostname = "172.217.162.174";
     char proxy_port[100];

     //strcpy(ip,argv[1]); // IP do Server
     strcpy(port,argv[1]);  // Porta do Servidor
     strcpy(proxy_port,argv[2]); // Porta do Proxy

      printf("IP do server: %s na porta %s \n", ip, port);
      printf("Porta do proxy: %s \n", proxy_port);

      //Variaveis do Socket
      int proxy_fd = 0, client_fd = 0;
      struct sockaddr_in proxy_sd;

      signal(SIGPIPE,SIG_IGN);

      /* Criando o Socket */
      if ((proxy_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
      {
          /* IPV4, TCP, IP = 0 */
          printf("Socket Falhou!\n");
      }
      else printf("Socket Criado com Sucesso!\n");

      proxy_sd.sin_family = AF_INET;  /* Protocolo IPV4 */
      proxy_sd.sin_addr.s_addr = INADDR_ANY;
      proxy_sd.sin_port = htons(atoi(proxy_port));


      if (bind(proxy_fd, (struct sockaddr*)&proxy_sd, sizeof(proxy_sd))<0)
      {
          printf("Bind falhou!\n");
      }
      if (listen(proxy_fd, 4) < 0)
      {
          printf("Não foi possível escutar!\n");
          exit(1);
      }
      printf("Esperando conexao..\n");

      //Aceitar todas as conexoes com clientes continuamente
      while(1)
      {
           client_fd = accept(proxy_fd, (struct sockaddr*)NULL ,NULL);
           printf("Cliente num. %d se conectou! \n",client_fd);
           if(client_fd > 0)
           {
                 //Variaveis do Multithread
                 struct serverInfo *item;
                 item = (serverInfo*) malloc(sizeof(struct serverInfo));
                 item->client_fd = client_fd;
                 strcpy(item->ip,ip);
                 strcpy(item->port,port);
                 pthread_create(&tid, NULL, runSocket, (void *)item);
                 sleep(1);
           }
      }
      return 0;
 }

 void *runSocket(void *vargp)
 {
    struct serverInfo *info = (struct serverInfo *)vargp;

    char buffer[BUFFERSIZE];
    int bytes =0;
       printf("Cliente: %d\n",info->client_fd);
       //fputs(info->ip,stdout);
       //fputs(,stdout);
       printf("IP do server: %s na porta %s \n", info->ip, info->port);
       //code to connect to main server via this proxy server
       int server_fd =0;
       struct sockaddr_in server_sd;
       // create a socket

       server_fd = socket(AF_INET, SOCK_STREAM, 0);
       if(server_fd < 0)
       {
            printf("server socket not created\n");
       }

       printf("Socket do Servidor Criado!\n");
       memset(&server_sd, 0, sizeof(server_sd));
       // set socket variables
       server_sd.sin_family = AF_INET;
       server_sd.sin_port = htons(atoi(info->port));
       server_sd.sin_addr.s_addr = inet_addr(info->ip);
       //connect to main server from this proxy server

       if((connect(server_fd, (struct sockaddr *)&server_sd, sizeof(server_sd)))<0)
       {
            printf("server connection not established");
       }

       printf("Socket do Servidor Conectado!\n");
       while(1)
       {
            //receive data from client
            memset(&buffer, '\0', sizeof(buffer));
            bytes = read(info->client_fd, buffer, sizeof(buffer));
            if(bytes <= 0)
            {
            }
            else
            {

                 //Data p/ server
                 write(server_fd, buffer, sizeof(buffer));
                 //printf("client fd is : %d\n",c_fd);
                 printf("From client :\n");
                 fputs(buffer,stdout);
                   fflush(stdout);
            }
            //Resposta do server

            memset(&buffer, '\0', sizeof(buffer));
            bytes = read(server_fd, buffer, sizeof(buffer));
            if(bytes <= 0)
            {
            }
            else
            {

                 // Resposta pro client
                 write(info->client_fd, buffer, sizeof(buffer));
                 printf("From server :\n");
                 //fputs(buffer,stdout);

            }
       };
    return NULL;
  }

 int hostname_to_ip(char * hostname , char* ip)
 {
   struct hostent *he;
   struct in_addr **addr_list;
   int i;
   if ( (he = gethostbyname( hostname ) ) == NULL)
   {

     //Informação do Host BUG

     herror("gethostbyname");
     return 1;
   }
   addr_list = (struct in_addr **) he->h_addr_list;
   for(i = 0; addr_list[i] != NULL; i++)
   {

     //Retorna primeiro endereço da address list BUG

     strcpy(ip , inet_ntoa(*addr_list[i]) );
     return 0;
   }
   return 1;
 }
