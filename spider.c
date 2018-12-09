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

void spider();
int fileSearch(char *, FILE *);
int SearchFile(char *str, FILE *fp);

int cont = 1;
/*
void parsing(char*, char*, char*);
int get_host_by_name(char*, char*);
void make_list(char*,struct Node*);*/

struct No{
 char href[HREFSIZE];
 struct No *prox;
};
typedef struct No no;

int main(){

  spider();

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

  no *href_root;
  href_root = (no *) malloc(1*sizeof(no));

  char c, *href_text = "href=", *src_text = "src=";
  char *href_result;
  int i = 1, filesearch;

  href_result = (char *) malloc(HREFSIZE*sizeof(char));

  while(!feof(website_file)){
    filesearch = SearchFile(href_text, website_file);
    if(filesearch == 0){
      fread(&c, 1, 1, website_file);
      href_result[0] = c;
      while(c != '"'){
        fread(&c, 1, 1, website_file);
        href_result[i++] = c;
      }
      href_result[i-1] = '\0';
      printf("***HREF***: %s\n", href_result);
      fprintf(tree_file, "CONTADOR  = %d -> href=%s\n", cont++, href_result);
      bzero(href_result, HREFSIZE);
    }
    i = 1;
  }
/*
  rewind(website_file);

  while(!feof(website_file)){
    filesearch = fileSearch(href_text, website_file);
    if(filesearch == 0){
      fread(&c, 1, 1, website_file);
      fread(&c, 1, 1, website_file);
      href_result[0] = c;
      while(c != '"'){
        fread(&c, 1, 1, website_file);
        href_result[i++] = c;
      }
      href_result[i-1] = '\0';
      printf("***HREF***: %s\n", href_result);
      fprintf(tree_file, "href=%s\n", href_result);
    }
    i = 1;
  }*/

}

int SearchFile(char *str, FILE *fp)
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
