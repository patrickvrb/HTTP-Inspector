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
int searchFile(char *str, FILE *fp);
list *createNode();
int hrefType(/*char *href, char *hostname*/);

int cont = 1;

int main(){

  //spider();
  hrefType();

  return 0;
}

void spider(){

  FILE *tree_file, *website_file;

  website_file = fopen("website_file.txt", "r");
  tree_file = fopen("html_tree.txt", "w");

  if(tree_file == NULL || website_file == NULL){
      printf("Erro ao abrir arquivos do spider\n");
      exit(-1);
  }

  list *href_root;
  href_root = createNode();


  char c, *href_text = "href=", *src_text = "src=";
  char *href_result;
  int i = 1, filesearch;

  href_result = (char *) malloc(HREFSIZE*sizeof(char));

  while(!feof(website_file)){
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
      bzero(href_result, HREFSIZE);
    }
    i = 1;

    filesearch = searchFile(src_text, website_file);
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
      printf("***SRC***: %s\n", href_result);
      fprintf(tree_file, "CONTADOR  = %d -> src=%s\n", cont++, href_result);
      bzero(href_result, HREFSIZE);
    }

    if(feof(website_file))
      break;
  }

  fclose(website_file);
  fclose(tree_file);

}

int hrefType(/*char *href, char *hostname*/){

  char *href = "http://www.teste.com.br/lel";
  char *hostname = "www.teste.com.br";

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

  if(strcmp(extracted,hostname) == 0){
    printf("Mesmo domínio!\n");
    return 0;
  }

  /* Concatena com o hostname pai atual */
  char *conct;
  conct = (char *) malloc (100*sizeof(char));
  strcpy(conct,hostname);
  if(href[0] != '/')
    strcat(conct, "/");
  strcat(conct,href);
  printf("Concatenamento: %s\n", conct);
  return 1;

}

int searchFile(char *str, FILE *fp)
{

  char str_c, file_c;
  char *str_equal;
  int i=0, j=1;

  str_equal = (char *) malloc(10*sizeof(char));

  do{
    fread(&file_c, 1, 1, fp);
    if(file_c == str[i]){
      str_equal[i] = file_c;
      i++;
    } else i = 0;

    if(i > strlen(str))
      break;

  }while(strcmp(str, str_equal) != 0);

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
  //href_new->href = "0";
  href_new->next = NULL;
  href_new->son = NULL;
  return href_new;
}
