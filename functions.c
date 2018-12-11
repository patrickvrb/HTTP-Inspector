#include "headers.h"

int cont = 1;
int namecounter = 0;
int cache_cont = 1;
char *href_text = "href=", *src_text = "src=";

void createFiles(char *wf_name, FILE *website_file, char *tf_name, FILE *tree_file)
{
    website_file = fopen(wf_name, "r");
    tree_file = fopen(tf_name, "w");

    if (website_file == NULL)
    {
        printf("Erro ao abrir arquivos do spider(wf)\n");
        exit(-1);
    }

    if (tree_file == NULL)
    {
        printf("Erro ao abrir arquivos do spider(tf)\n");
        exit(-1);
    }
}

/*
 * Funcao que envia o request do Browser para o servidor.
 * Ela cria outra conexao na porta 80 e envia o segmento HTTP contido em request
 * Antes, ela deve remontar o pedido HTTP e descobrir o IP do site atraves do Hostname
 */
void server_response(char *request, int mode)
{
    char *hostname, *server_ip, c;
    char newrequest[300];
    
    printf("\nREQUEST MODO 1:%s", request);
    int i = 1, j = 1;

    c = request[0];
    hostname = (char *)malloc(HOSTSIZE * sizeof(char));
    server_ip = (char *)malloc(33 * sizeof(char));
    //printf("\nREQUEST SPIDER:%s-\n", request);
    /* Achar o hostname a partir do request */
    
    if(mode == 0) {
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
        
    }
    else if (mode == 1) {
        strtok(request, "/");
        printf("Hostname: %s\n", hostname);
    }
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
    response = (char *)malloc(BUFFERSIZE * sizeof(char));

    //Montando Request no formato padrão

    switch (findMethod(request, mode))
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

    char *wf_name;
    wf_name = (char *)malloc(HOSTSIZE*sizeof(char));
    sprintf(wf_name, "temp/website_file_%d.txt", namecounter);

    website_file = fopen(wf_name, "w");
    if (website_file == NULL)
    {
        printf("Erro ao criar o Website File\n");
        exit(-1);
    }

    do 
    {
        bzero(response, BUFFERSIZE);
        read(proxy_server_socket, response, BUFFERSIZE);
        fputs(response, website_file);
    }while(strstr(response, "</html>") == NULL);

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

int findMethod(char *str, int mode)
{
    if(mode == 0) {
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
    } else{
        /* ALWAYS GET ? */
        return 0;
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

void hrefBuilder(char *website_file, char *tree_file)
{
    FILE *wf_name, *tf_name;

    wf_name = fopen(website_file, "r");
    tf_name = fopen(tree_file, "w");
    char *href_result;
    char c;

    do
    {
        href_result = findHref(wf_name, 0);
        printf("***HREF***: %s\n", href_result);
        fprintf(tf_name, "%d -> href=%c%s%c\n", cont++, '"', href_result, '"');
        c = (char)fgetc(wf_name);
    } while (c != EOF);

    rewind(wf_name);

    do
    {
        href_result = findHref(wf_name, 1);
        printf("***SRC***: %s\n", href_result);
        fprintf(tf_name, "%d -> src=%c%s%c\n", cont++, '"', href_result, '"');
        c = (char)fgetc(wf_name);
    } while (c != EOF);
                
    fclose(wf_name);
    fclose(tf_name);
}

char *findHref(FILE *fp, int mode)
{

    int filesearch;
    int i = 1;
    char c;

    char *href_result;

    href_result = (char *)malloc(HREFSIZE * sizeof(char));

    if (mode == 0)
        filesearch = searchFile(href_text, fp);
    else if (mode == 1)
        filesearch = searchFile(src_text, fp);

    if (filesearch == 0)
    {
        fread(&c, 1, 1, fp);
        href_result[0] = c;
        while (c != '"')
        {
            if (feof(fp))
                break;
            fread(&c, 1, 1, fp);
            href_result[i++] = c;
        }
        href_result[i - 1] = '\0';
    }

    return href_result;
}

char *extractDomain(char *hostname)
{

    int j = 0, i = 0;
    char c;
    char *extracted;
    extracted = (char *)malloc((strlen(hostname)) * sizeof(char));

    c = hostname[0];
    while (c != '/') // Caminha até a primeira barra
    {
        c = hostname[i++];
    }

    c = hostname[i++];
    c = hostname[i++];
    extracted[0] = c;
    while (c != '/')
    {
        c = hostname[i++];
        extracted[j++] = c;
        //printf("Char %c!\n", c);
        if (i == strlen(hostname))
            break;
    }
    extracted[i - 1] = '\0';
    strtok(extracted, "/");

    printf("String: %s\n", extracted); //flaviomoura.mat.br/files/lel
    return extracted;
}

int hrefType(char *href, char *hostname, int is_src)
{
    /* Descarta referencias (#)*/
    if (href[0] == '#')
    {
        printf("Descartado! (#)\n");
        return -1;
    }

    if (strlen(href) == 1 && href[0] == '/')
    {
        printf("Descartado! (/)\n");
        return -1;
    }
    /* Ver se é um link do mesmo domínio*/

    int flag = 0;

    if((strstr(href, "http") != NULL) || (strstr(href, "https") != NULL))
        flag = 1;

    char *extracted_href, *extracted_hostname;

    extracted_href = extractDomain(href);
    extracted_hostname = extractDomain(hostname);

    //TODO: Mesmo domínio ainda não é o suficiente.
    // A gente precisa ver se é filho ou é next
    if (strcmp(extracted_href, extracted_hostname) == 0)
    {
        printf("Mesmo domínio!\n");
        return 0;
    }
    else if (flag == 1)
        return -1;

    if (is_src == 1)
        return is_src;

    /* Concatena com o hostname pai atual */
    return 2;
}

list *treeMaker(char *href, char *hostname, list *href_root, int mode)
{

    list *href_aux = href_root;
    list *href_new;
    href_new = createNode();
    strcpy(href_new->href, href);

    char *conct, *result;
    conct = (char *)malloc(HOSTSIZE * sizeof(char));
    result = (char *)malloc(HOSTSIZE * sizeof(char));

    char c;
    int i = 1;

    switch (mode)
    {
    case -1:
        /* Ignora */
        return NULL;

    case 0:
        /* Criar um filho na lista */
        if (href_aux->son == NULL)
        {
            href_aux->son = href_new;
            return href_aux->son;
        }
        // Se não é a primeira vez, checar a ocorrência do href/src_text
        else if (searchList(href, href_aux) == NULL)
        {
            // Não achou ocorrência na lista
            while (href_aux->son != NULL)
                href_aux = href_aux->son;
            href_aux->son = href_new;
            return href_aux->son;
        }
        return NULL;

    case 1:
        /* Concatena o src recebido com o hostname atual */
        c = href[0];
        conct[0] = c;
        while (c != ';') // Caminha até ;
        {
            c = href[i];
            conct[i] = c;
            i++;
        }
        conct[i - 1] = '\0';
        //printf("Resultado: %s \n", conct);

        strcpy(result, hostname);
        if (href[0] != '/')
            strcat(result, "/");
        strcat(result, conct);
        strcpy(href_new->href, result);
        return href_new;

    case 2:
        /* Concatena o href recebido com o hostname atual */
        strcpy(conct, hostname);
        if (href[0] != '/')
            strcat(conct, "/");
        strcat(conct, href);
        printf("Concatenamento: %s\n", conct);
        strcpy(href_new->href, conct);
        return href_new;

    default:
        return NULL;
    }
}

void saveTree(list *href_root, FILE *tree_file)
{
    fprintf(tree_file, "href=%s\n", href_root->href);

    if (href_root->son != NULL)
    {
        saveTree(href_root->son, tree_file);
    }
    if (href_root->next != NULL)
    {
        saveTree(href_root->next, tree_file);
    }
}

/*
 *  Função que procura uma determinada ocorrência na lista inteira
 *  Retorna o ponteiro da ocorrência ou NULL se nenhuma ocorrência foi achada
 */
list *searchList(char *href, list *href_root)
{

    list *href_result = NULL;

    if (href_root == NULL)
        return href_root;

    if (strcmp(href_root->href, href) == 0)
    {
        return href_root;
    }
    else
    {
        if (href_root->son != NULL)
            href_result = searchList(href, href_root->son);
        if (href_result != NULL)
            return href_result;
        if (href_root->next != NULL)
            href_result = searchList(href, href_root->next);
        if (href_result != NULL)
            return href_result;
        return NULL;
    }
}

int searchFile(char *str, FILE *fp)
{

    char file_c;
    char *str_equal;
    int i = 0;

    str_equal = (char *)malloc(10 * sizeof(char));

    do
    {

        if (feof(fp))
            break;

        fread(&file_c, 1, 1, fp);
        if (file_c == str[i])
        {
            str_equal[i] = file_c;
            i++;
        }
        else
            i = 0;

        if (i > strlen(str))
            break;

    } while (strcmp(str, str_equal) != 0);

    if (feof(fp))
        return -1;

    fread(&file_c, 1, 1, fp);
    return 0;
}

list *createNode(void)
{
    list *href_new;
    href_new = (list *)malloc(sizeof(list));
    if (href_new == NULL)
    {
        printf("Erro ao alocar memória\n");
        exit(-1);
    }
    href_new->next = NULL;
    href_new->son = NULL;
    return href_new;
}
