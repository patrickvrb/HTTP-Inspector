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
#include <fcntl.h>

#define PORT 8228 /* Porta Proxy Local 8228 */
#define BUFFERSIZE 4096
#define HREFSIZE 65535
#define HOSTSIZE 100
#define h_addr h_addr_list[0]

struct List
{
  char href[HREFSIZE];
  struct List *next;
  struct List *son;
};
typedef struct List list;
void server_response(char *, int);
void spider(char *, list *);
void hrefBuilder(FILE *, FILE *);
int searchFile(char *, FILE *);
list *searchList(char *, list *);
list *createNode(void);
int hrefType(char *, char *, int);
list *treeMaker(char *, char *, list *, int);
void saveTree(list *, FILE *);
void get_ip(char *, char *);
char *load_cache(char *, int);
int save_cache(char *, int);
int fileSearch(char *, FILE *);
int findMethod(char *, int);
char *findHref(FILE *, int );
void createFiles(char *, FILE *, char *, FILE *);

//mega EASTER EGG