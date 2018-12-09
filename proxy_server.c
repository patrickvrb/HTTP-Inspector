/*
 * INSPETOR HTTP
 * Danillo Neves Souza        - 14/0135839
 * Patrick Vitas Reguera Beal - 15/0143672
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

/* Cabecalho das Funcoes */
void server_response(char[]);
void get_ip(char *, char *);
char *load_cache(char *r, int);
int save_cache(char *, int);
int fileSearch(char *, FILE *);
void sendBrowser(char *);
int findMethod(char *str);

/* Variavel global para facilitar o save e o load da cache */
int cache_cont = 1;

int main(int argc, char const *argv[])
{

    int browser_socket, porta = 0, tr = 1;
    int browser_conn;
    char request[BUFFERSIZE], *response;
    long message_status;

    printf("\n*******   HTTP INSPECTOR   *******\n");
    printf("By: Danillo & Patrick\n");
    printf("Teleinformática e Redes 2 - UnB\n");
    printf("Deseja se conectar em qual porta? (0 - Padrão)   ");
    scanf("%d", &porta);

    if (porta == 0)
    {
        printf("Porta padrão 8228 selecionada!\n");
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

    /* Liberando Socket utilizado em sessões anteriores */
    if (setsockopt(browser_socket, SOL_SOCKET, SO_REUSEADDR, &tr, sizeof(int)) == -1)
    {
        perror("Erro com liberação do Socket!\n");
        exit(1);
    }

    /* Definindo Variaveis do Socket */
    struct sockaddr_in browser_addr;
    browser_addr.sin_family = AF_INET;
    browser_addr.sin_addr.s_addr = INADDR_ANY;
    browser_addr.sin_port = htons(porta);

    /* Realizando o Bind na Porta Selecionada */
    if (bind(browser_socket, (struct sockaddr *)&browser_addr, sizeof(browser_addr)) < 0)
    {
        printf("Bind falhou!\n");
        exit(-1);
    }

    /* Escutando o Socket Criado */
    if (listen(browser_socket, SOMAXCONN) < 0)
    {
        printf("Não foi possível escutar!\n");
        exit(1);
    }

    response = (char *)malloc(BUFFERSIZE * sizeof(char));

    while (1)
    {
        /* Aceita qualquer conexão vindo do Socket*/

        browser_conn = accept(browser_socket, (struct sockaddr *)NULL, NULL);

        if (browser_conn <= 0)
        {
            printf("Erro ao aceitar conexão com o Browser!\n");
            exit(-1);
        }
        else
        {
            printf("Conexão aceita! \n");

            //Recebe Request do Browser
            memset(&request, '\0', sizeof(request));
            message_status = recv(browser_conn, request, sizeof(request), 0);

            if (message_status < 0)
            {
                perror("Read Error");
            }
            else
            {
                /* Envia o Request do Browser ao Servidor */
                server_response(request);
                /* Abertura da Cache local para ser lida */
                FILE *website_file;
                website_file = fopen("website_file.txt", "r");
                if (website_file == NULL)
                {
                    printf("Erro! Cache não encontrada!\n");
                    exit(-1);
                }
                printf("Resposta do Servidor:\n");
                while (fread(response, 1, sizeof(response), website_file) <= sizeof(response))
                {
                    printf("%s", response);
                    message_status = send(browser_conn, response, sizeof(response), 0);
                    bzero(response, BUFFERSIZE);
                }
                printf("\nEscutando...\n");
                fclose(website_file);
            }
            fflush(stdout);
        }
    }

    return 0;
}

/*
 * Funcao que envia o request do Browser para o servidor.
 * Ela cria outra conexao na porta 80 e envia o segmento HTTP contido em request
 * Antes, ela deve remontar o pedido HTTP e descobrir o IP do site atraves do Hostname
 */

void server_response(char *request)
{
    char *hostname, *server_ip, c;
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
    printf("IP Address: %s\n", server_ip);
    free(server_ip);
    int proxy_server_socket, lak = 1;
    if ((proxy_server_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
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
    response = (char *)malloc(BUFFERSIZE*sizeof(char));

    //Montando Request no formato padrao

    switch (findMethod(request))
    {
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
    strcat(newrequest, "/ HTTP/1.1\r\nHost: ");
    strcat(newrequest, hostname);
    strcat(newrequest, "\r\n\r\n");
    //printf("PROXY REQUEST: %s\n", newrequest);
    free(hostname);
    write(proxy_server_socket, newrequest, sizeof(newrequest));
    bzero(response, BUFFERSIZE);

    FILE *website_file;

    website_file = fopen("website_file.txt", "w");
    if (website_file == NULL)
    {
        printf("Erro ao criar a Cache!\n");
        exit(-1);
    }

    while (read(proxy_server_socket, response, BUFFERSIZE))
    {
        fputs(response, website_file);
        bzero(response, BUFFERSIZE);
    }
    close(proxy_server_socket);
    fclose(website_file);
    free(response);
}

/*
 * Funcao que descobre se o segmento HTTP e GET ou POST e retorna
 * 1 - POST
 * 0 - GET
 * -1 - Erro
 */

int findMethod(char *str)
{
    if (str[0] == 'P')
    {
        return 1;
    }
    else if (str[0] == 'G')
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

/*
 * Funcao que descobre o IP do servidor atraves do Hostname
 * A funcao gethostbyname faz todo o trabalho
 */
void get_ip(char *hostname, char *ip)
{
    struct hostent *he;
    struct in_addr **addr_list;

    if ((he = gethostbyname(hostname)) == NULL)
    {
        herror("gethostbyname");
        exit(1);
    }

    addr_list = (struct in_addr **)he->h_addr_list;
    strcpy(ip, inet_ntoa(*addr_list[0]));

}

/*
 * Funcao que salva os segmentos HTTP em um arquivo .txt
 */
int save_cache(char *str, int mode)
{
    FILE *cache;

    cache = fopen("cache.txt", "a+");
    if (cache == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        return -1;
    }

    switch (mode)
    {
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

/*
 * Funcao que carrega os segmentos HTTP de um arquivo txt
 * Ela retorna o segmento que for igual ao argumento passado
 */
char *load_cache(char *str, int mode)
{
    FILE *cache;

    cache = fopen("cache.txt", "a+");
    if (cache == NULL)
    {
        printf("Erro ao abrir o arquivo\n");
        exit(-1);
    }

    char *http_data, c = str[0];
    http_data = (char *)malloc(BUFFERSIZE * sizeof(char));

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
    //strtok(hostname, "/");

    switch (mode)
    {
        case 0:
            if (fileSearch(http_data, cache) == 0)
            {
                return http_data;
                break;
            }
            else
            {
                printf("Nada no cache\n");
                free(http_data);
                char *nothing = NULL;
                return nothing;
            }
        default:
            printf("Modo cache inexistente\n");
            free(http_data);
            char *nothing = NULL;
            return nothing;
            break;
    }

    fclose(cache);
    free(http_data);
}

/*
 * Funcao que acha uma determinada string em um arquivo
 * Retorna 0 se encontrar algum trecho igual
 */
int fileSearch(char *str, FILE *fp)
{

    char str_c, file_c;
    int i = 1, flag = 1;
    unsigned long str_len;

    str_len = strlen(str);
    //printf("Tamanho: %d\n", str_len);

    printf("String: %s\n", str);

    while (!feof(fp))
    {
        fread(&file_c, 1, 1, fp);
        str_c = str[0];
        while (str_c != file_c)
        {
            fread(&file_c, 1, 1, fp);
        }

        printf("Iguais: %c e %c\n", str_c, file_c);

        while (flag == 1)
        {
            fread(&file_c, 1, 1, fp);
            str_c = str[i++];
            printf("String: %c e File: %c\n", str_c, file_c);
            if (str_c != file_c)
            {
                break;
            }
            else
            {
                if (i == str_len)
                    return 0;
            }
        }
        break;
    }
    return -1;
}

/*
 * Funcao que envia a string passada para uma porta pre-determinada
 * Foi implementada para tentar resolver o erro do Browser nao carregar
 * Nao da certo com 8228, mas da certo com o localhost
 */
void sendBrowser(char *str)
{

    char *http_data, c = str[0];
    http_data = (char *)malloc(BUFFERSIZE * sizeof(char));
    int i = 1, j = 2;
    long message_status;

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
    free(http_data);
    int proxy_socket, sockopt = 1;

    if ((proxy_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        /* IPV4, TCP, IP = 0 */
        printf("Socket do Browser sFalhou!\n");
        exit(1);
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

    message_status = write(proxy_socket, http_header, sizeof(http_header));
    if (message_status < 0)
    {
        perror("Erro no envio ao Browser");
    }
    else
    {
        printf("Pedido Encaminhado ao Browser\n");
    }
}
