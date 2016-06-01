#include "csapp.h"
#include <string.h>

/* usage: ./echoclient host port */
int portChecker(char* port);
int format_log_entry(char* date, char* ip, char* url, char* size);
int main(int argc, char **argv)
{
	//"server" stuff referrs to connecting to webserver, "browser" variables refer to connecting to user's browser, anything else is either local proxy data or i've forgotten to change the name
	int serverfd, browserfd, browserreadfd, clientlen;
	struct sockaddr_in clientaddr;
	char *host, serverBuffer[MAXLINE], hostBuffer[MAXLINE], header[MAXLINE], date[MAXLINE], requestAddr[MAXLINE];	//header not necessary if commented out if/else isn't needed, delete if that's the case
	char *stringIP;
	stringIP = malloc(20);
	struct hostent *ip;
	if(portChecker(argv[1]) == -1){
		return;	//port is reserved, error message given in function
	}
	rio_t serverData;
	rio_t browserData;
	//rio_t browserData_init;
    browserfd = Open_listenfd(argv[1]);
	while(1){
		//serverfd = Open_clientfd(argv[1], argv[2]);

		int bufferSize, i;
		char temp[MAXLINE];
		browserreadfd = Accept(browserfd, (SA *)&clientaddr, &clientlen);	//then just do stuff with clientaddr for logging, see main.c for info on that

		//Rio_readinitb(&serverData, serverfd);
		Rio_readinitb(&browserData, browserreadfd);
		//Rio_readinitb(&browserData_init, browserreadfd);


		//set up connection to host
		Rio_readlineb(&browserData, serverBuffer, MAXLINE);
		//Fputs(serverBuffer, stdout);
		Rio_readlineb(&browserData, hostBuffer, MAXLINE);
		//Fputs(hostBuffer, stdout);
		host = hostBuffer+6;
		int hlen = strlen(host);
		host[hlen-1] = '\0';
		host[hlen-2] = '\0';
		//printf("ATTEMPT CONN: %s\n", host);
		char connport[3]; connport[0]='8';connport[1]='0';connport[2]='\0';
		serverfd = Open_clientfd(host, connport);
		Rio_readinitb(&serverData, serverfd);
		Rio_writen(serverfd, serverBuffer, strlen(serverBuffer));
		Rio_writen(serverfd, hostBuffer, strlen(hostBuffer));
		strcpy(requestAddr, serverBuffer);
		strcpy(requestAddr, strtok(requestAddr, " \n\t\r"));
		strcpy(requestAddr, strtok(NULL, " \n\t\r"));
		//printf("url request now |%s|\n", requestAddr);
		//printf("CONNECTION ESTABLISHED TO : %s\n", host);
		//ip = Gethostbyname((const char *)&clientaddr.sin_addr.s_addr, sizeof(clientaddr.sin_addr.s_addr), AF_INET);
		//ip = Gethostbyname((const char *)&clientaddr.sin_addr.s_addr);
		Inet_ntop(AF_INET, &clientaddr.sin_addr.s_addr, stringIP, 20);
		printf("IP now |%s|\n", stringIP);
		//now send the remainder of requests to the server
		//printf(":"); fflush(stdout);

		//int i = 0;
		int checkStatus = 1;
		bufferSize = 0;
		while (checkStatus!=0) {
			Rio_readlineb(&browserData, serverBuffer, MAXLINE);	//GET request from browser
			//Fputs(serverBuffer, stdout);

			if(strstr(serverBuffer, "Content-Length:")){
				strcpy(temp,serverBuffer);
				strcpy(temp, strtok(temp, " \n\t\r"));
				strcpy(temp, strtok(NULL, " \n\t\r"));
				bufferSize = atoi(temp);
			}
			Rio_writen(serverfd, serverBuffer, strlen(serverBuffer));  //sends data to server, get rid of placeholder code above this
			if(strcmp(serverBuffer, "\r\n") == 0) break;
		}

		if(bufferSize > 0){
			for(i = 0; i < bufferSize; i++){
				Rio_readnb(&browserData,serverBuffer,1);
				//printf("%c",serverBuffer[0]);
				Rio_writen(serverfd, serverBuffer, 1);
			}
		}

		//printf("IF IT HANGS HERE, IT'S OPENLAB'S FAULT\n");
		Rio_readlineb(&serverData,serverBuffer,MAXLINE);
		checkStatus = 1;
		bufferSize = 0;

		//TODO: ALL fputs should be changed to write back to client instead of to stdout (but for testing purposes this works)
		Rio_writen(browserreadfd, serverBuffer, strlen(serverBuffer));
		while(checkStatus != 0){
			Rio_readlineb(&serverData, serverBuffer, MAXLINE);	//RESPONSE message
				//Fputs(serverBuffer,stdout);
				Rio_writen(browserreadfd, serverBuffer, strlen(serverBuffer));
				if(strcmp(serverBuffer,"\r\n")== 0){	//how to detect end of header
				  checkStatus = 0;
				  }
			else if(strstr(serverBuffer, "Content-Length:")){
				strcpy(temp,serverBuffer);
				strcpy(temp, strtok(temp, " \n\t\r"));
				strcpy(temp, strtok(NULL, " \n\t\r"));
				bufferSize = atoi(temp);
			}
			else if(strstr(serverBuffer, "Date:")){
				strcpy(date, serverBuffer);
				strcpy(date, strtok(date, " \n\t\r"));
				strcpy(date, strtok(NULL, "\r\n"));
			}
			//just loop through header, check relevant lines to ensure no errors, and find size of main body
			//then just loop until there's an empty line, then start listing characters in for loop below
		}
		//printf("reading message of: %d bytes\n", bufferSize);
		//}
		i = 0;
		if(bufferSize > 0){
			for(i = 0; i < bufferSize; i++){
				Rio_readnb(&serverData,serverBuffer,1);
				//printf("%c",serverBuffer[0]);
				Rio_writen(browserreadfd, serverBuffer, 1);
			}
		}
		Rio_writen(browserreadfd, "\r\n", 2);
		printf("wrote %d bytes to client\n", bufferSize);
		
		//printf("logging here, with date = |%s|, size = |%d|, domain = |%s|, and IP = |%s|\n", date, bufferSize, requestAddr, stringIP);
		format_log_entry(date, stringIP, requestAddr, temp);
		Close(serverfd);
		//Close(browserfd);
		Close(browserreadfd);
	}
	free(stringIP);
    	exit(0);
}

/*
portchecker
takes port as string of chars
loops through /etc/services searching for the port number
if found, port is reserved for another service, and function returns -1
otherwise, returns 0, indicating port is ok to use
*/
int portChecker(char* port){     //TODO: actually implement this
	char test[80];
	char* temp = malloc(80);
	char* reader = malloc(80);
	FILE* inputFile;
	test[0] = ' ';
	strcat(test,port);
	strcat(test, "/");
	inputFile = fopen("/etc/services","r");
	while(fgets(reader, 80, inputFile) != NULL){
		if(strstr(reader, test)){
			printf("Port Reserved\n");
			fclose(inputFile);
			return -1;
		}
	}
	fclose(inputFile);
	return 0;
}

int format_log_entry(char* date, char* ip, char* url, char* size) {
	FILE *outfile = Fopen("proxy.log", "a");
	Fputs(date, outfile);
	Fputs(": ", outfile);
	Fputs(ip, outfile);
	Fputs(" ", outfile);
	Fputs(url, outfile);
	Fputs(" ", outfile);
	Fputs(size, outfile);
	Fputs("\n", outfile);
	fclose(outfile);
	return 0;
}
