/*
 * INSPETOR HTTP - Spider.c
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
#define HREFSIZE 65535

struct List{
 char href[HREFSIZE];
 struct List *next;
 struct List *son;
};
typedef struct List list;

void spider();
int searchFile(char *, FILE *);
list *searchList(char *, list *);
list *createNode();
int hrefType(char *, char *);
list *treeMaker(char *, list*, int);

int cont = 1;

char *hostname = "flaviomoura.mat.br";

int main(){

  spider();
  //hrefType();

  return 0;
}

void spider(){

  FILE *tree_file, *website_file;

  website_file = fopen("website_file.txt", "r");
  tree_file = fopen("html_tree.txt", "w");

  if(tree_file == NULL) {
      printf("Erro ao abrir arquivos do spider(1)\n");
      exit(-1);
  }

  if(website_file == NULL){
    printf("Erro ao abrir arquivos do spider(2)\n");
    exit(-1);
  }

  list *href_root;
  href_root = createNode();

  char c, *href_text = "href=", *src_text = "src=";
  char *href_result;
  int i = 1, filesearch, hreftype;

  href_result = (char *) malloc(HREFSIZE*sizeof(char));

  //TODO: COMO DESCOBRIR O HOSTNAME ATUAL??

  do{
    filesearch = searchFile(href_text, website_file);
    if(filesearch == 0){
      fread(&c, 1, 1, website_file);
      href_result[0] = c;
      while(c != '"'){
        if(feof(website_file))
          break;
        fread(&c, 1, 1, website_file);
        href_result[i++] = c;
      }
      href_result[i-1] = '\0';
      printf("***HREF***: %s\n", href_result);
      fprintf(tree_file, "CONTADOR  = %d -> href=%s\n", cont++, href_result);
      //hreftype = hrefType(href_result, hostname);
      //treeMaker(href_result, href_root, hreftype);
    }
    i = 1;
    c = (char) fgetc(website_file);

  }while(c != EOF);

  fclose(website_file);
  fclose(tree_file);

}

int hrefType(char *href, char *hostname){

  //char *href = "http://flaviomoura.mat.br/lel";

  /* Descarta referẽncias (#)*/
  if(href[0] == '#'){
      printf("Descartado!\n");
      return -1;
  }

  /* Ver se é um link */
  char c;
  int i = 1, j = 1;

  char *extracted;
  extracted = (char *) malloc((strlen(href))*sizeof(char));

  c = href[0];
  while(c != '/'){
    c = href[i++];
  }

  c = href[i++];
  c = href[i++];
  extracted[0] = c;
  while(c != '/'){
    c = href[i++];
    extracted[j++] = c;
    printf("Char %c!\n", c);
    if(i == strlen(href))
      break;
  }
  extracted[i-1] = '\0';
  strtok(extracted,"/");

  printf("String: %s\n", extracted);

  //TODO: Mesmo domínio ainda não é o suficiente.
  // A gente precisa ver se é filho ou é next
  if(strcmp(extracted,hostname) == 0){
    printf("Mesmo domínio!\n");
    return 0;
  }

  /* Concatena com o hostname pai atual */
  return 1;

}

list *treeMaker(char *href, list *href_root, int mode){

  list *href_aux = href_root;

  list *href_new;
  href_new = createNode();
  strcpy(href_new->href, href);

  char *conct;

  switch(mode){
    case -1:
      /* Ignora */
      return NULL;
      break;
    case 0:
      /* Criar um filho na lista */
      if(href_aux->son == NULL){
        href_aux->son = href_new;
        return href_aux->son;
      } else {
        // Se não é a primeira vez, checar a ocorrência do href/src_text
        if(searchList(href, href_aux) == NULL){
          // Não achou ocorrência na lista
          while(href_aux->son != NULL)
            href_aux = href_aux->son;
          href_aux->son = href_new;
          return href_aux->son;
        }
      }
      break;
    case 1:
      conct = (char *) malloc (100*sizeof(char));
      strcpy(conct, hostname);
      if(href[0] != '/')
        strcat(conct, "/");
      strcat(conct,href);
      printf("Concatenamento: %s\n", conct);
      strcpy(href_new->href, conct);
      return href_new;
      break;
    default:
      break;
  }
}


/*
*  Função que procura uma determinada ocorrência na lista inteira
*  Retorna o ponteiro da ocorrência ou NULL se nenhuma ocorrência foi achada
*/
list *searchList(char *href, list *href_root){

  list *href_result;

  if(href_root == NULL)
    return href_root;

  if(strcmp(href_root->href, href) == 0){
    return href_root;
  } else {
    if(href_root->son != NULL)
      href_result = searchList(href, href_root->son);
    if(href_result != NULL)
      return href_result;
    if(href_root->next != NULL)
      href_result = searchList(href, href_root->next);
    if(href_result != NULL)
      return href_result;
    return NULL;
  }
}

int searchFile(char *str, FILE *fp)
{

  char str_c, file_c;
  char *str_equal;
  int i=0, j=1;

  str_equal = (char *) malloc(10*sizeof(char));

  do{

    if(feof(fp))
      break;

    fread(&file_c, 1, 1, fp);
    if(file_c == str[i]){
      str_equal[i] = file_c;
      i++;
    } else i = 0;

    if(i > strlen(str))
      break;

  }while(strcmp(str, str_equal) != 0);

  if(feof(fp))
    return -1;

  fread(&file_c, 1, 1, fp);
  return 0;
}

list *createNode(){
  list *href_new;
  href_new = (list *)malloc(sizeof(list));
  if(href_new == NULL){
    printf("Erro ao alocar memória\n");
    exit(-1);
  }
  href_new->next = NULL;
  href_new->son = NULL;
  return href_new;
}
