/*Local Proxy Library*/

// A structure to maintain client fd, and server ip address and port address
// client will establish connection to server using given IP and port
struct serverInfo
{
    int client_fd;
    char ip[100];
    char port[100];
};

int hostname_to_ip(char * , char *);

// A thread function
// A thread for each client request
void *runSocket(void *vargp);
