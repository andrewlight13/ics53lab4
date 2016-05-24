#include "csapp.h"
//TO SUCCESSFULLY COMPILE
//gcc -pthread -o test main.c csapp.c 
void echo(int connfd);

int main(int argc, char **argv) {
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;
    struct hostent *hp;
    char *haddrp;
    unsigned short client_port;
    port = atoi(argv[1]); /* the server listens on a port passed */     // on the command line */
    //listenfd = Open_listenfd(port); 
    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(clientaddr); 
        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        hp = Gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,
                        sizeof(clientaddr.sin_addr.s_addr), AF_INET);
        haddrp = inet_ntoa(clientaddr.sin_addr);
        client_port = ntohs(clientaddr.sin_port);
        printf("server connected to %s (%s), port %u\n",
                hp->h_name, haddrp, client_port);
        echo(connfd);
        Close(connfd);
    }
}
void echo(int connfd){
	size_t n;
	char buf[MAXLINE];
	rio_t rio;
	Rio_readinitb(&rio,connfd);
	while((n= Rio_readlineb(&rio, buf, MAXLINE)) != 0){
		printf("server received %d bytes\n", (int)n);
		Rio_writen(connfd, buf, n);
	}
}
