/*
 * INSPETOR HTTP - Spider.c
 * Danillo Neves Souza        - 14/0135839
 * Patrick Vitas Reguera Beal - 15/0143672
 */

#include "functions.c"

int main()
{
  system("mkdir temp");
  char *hostname = "flaviomoura.mat.br";
  //hostname = (char *) malloc(HOSTSIZE*sizeof(char));

  //printf("Digite a URL desejada para executar o Spider:\n");
  //fgets(hostname, HOSTSIZE, stdin);
  //strtok(hostname, "\0");

  list *href_root;
  list *href_son=NULL;

  href_root = createNode();
  strcpy(href_root->href, hostname);
  spider(hostname, href_root);
  //system("rm -rf temp");
  //Remove folder and contents: system("rm -rf <nomedapasta>")
}

void spider(char *hostname, list* href_root)
{
  FILE *tree_file, *website_file;
  list *href_temp = href_root;
  list *href_son = NULL;

  char *wf_name, *tf_name;
  wf_name = (char *) malloc(HOSTSIZE*sizeof(char));
  tf_name = (char *) malloc(HOSTSIZE*sizeof(char));

  sprintf(wf_name, "temp/website_file_%d.txt", namecounter);
  sprintf(tf_name, "temp/tree_file_%d.txt", namecounter);

  /* Server Response para o Host Atual */
  server_response(hostname, 1);
  namecounter++;
  printf("\nNAMECOUNTER: %d\n", namecounter);
  /* Criação de arquivos temporários */
  createFiles(wf_name, website_file, tf_name, tree_file);
    
  /* Monta os txts com todos os srcs e hrefs da resposta do servidor */
  hrefBuilder(wf_name, tf_name);
  char *href_atual;
  int hreftype;

  website_file = fopen("website_file.txt", "r");
  while(!(feof(website_file))){
    /* Acha o href do arquivo */
    href_atual = findHref(website_file, 0);
    printf("Href_Atual: %s\n", href_atual);

    /* Acha o modo de tratamento do href atual */
    hreftype = hrefType(href_atual, hostname, 0);

    /* Cria a lista a partir do modo de tratamento */
    href_son = treeMaker(href_atual, hostname, href_root, hreftype);

    /* Chamada recursiva */
    spider(href_son->href, href_son);

  }

  rewind(website_file);

  while(!(feof(website_file))){

    href_atual = findHref(website_file, 1);

    hreftype = hrefType(href_atual, hostname, 0);

    treeMaker(href_atual, hostname, href_root, hreftype);

    spider(href_atual, href_root);

  }

  //TODO: COMO DESCOBRIR O HOSTNAME ATUAL??
}