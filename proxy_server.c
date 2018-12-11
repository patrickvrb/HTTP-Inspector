/*
 * INSPETOR HTTP
 * Danillo Neves Souza        - 14/0135839
 * Patrick Vitas Reguera Beal - 15/0143672
 */

#include "functions.c"

/* Variavel global para facilitar o save e o load da cache */

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