#include "csapp.h" 

/* usage: ./echoclient host port */
int main(int argc, char **argv)
{ 
	int clientfd, port; 
	char *host, serverBuffer[MAXLINE], header[MAXLINE];	//header not necessary if commented out if/else isn't needed, delete if that's the case 
	rio_t serverData; 
	host = argv[1];  port = atoi(argv[2]); 
	clientfd = Open_clientfd(host, argv[2]);
	Rio_readinitb(&serverData, clientfd); 
	printf(":"); fflush(stdout);
	while (Fgets(serverBuffer, MAXLINE, stdin) != NULL) {
		Rio_writen(clientfd, serverBuffer, strlen(serverBuffer));  //sends data to server, get rid of placeholder code above this
		if(serverBuffer[0] == '\n') break;	//TODO: change this so it terminates on end of buffer (see  below)
		//if(strcmp(serverBuffer, "\r\n") == 0) break;
	}
	//check first line for 200 OK and HTTP version
	//ok not sure this is neccessary, so commented it out
	//Rio_readlineb(&serverData, header,MAXLINE);
	Rio_readlineb(&serverData,serverBuffer,MAXLINE);
    	int checkStatus = 1, bufferSize;
	char temp[MAXLINE];
	/*if(strstr(header,"HTTP/1.0 200 OK") == 0 && strstr(header,"HTTP/1.1 200 OK") == 0){	//if can't find either OK message
		Fputs(header,stdout);
		while(checkStatus != 0){	//loop through and print whole header
			Rio_readlineb(&serverData, serverBuffer, MAXLINE);
			Fputs(serverBuffer,stdout);
			if(strcmp(serverBuffer,"\r\n")== 0){	//how to detect end of header
			  checkStatus = 0;
			}
			else if(strstr(serverBuffer, "Content-Length:")){
				strcpy(temp,serverBuffer);
				strcpy(temp, strtok(temp, " \n\t\r"));
				strcpy(temp, strtok(NULL, " \n\t\r"));
				bufferSize = atoi(temp);
			}
			
		}
		 
		//Close(clientfd);
                //return;        //ideally, just print the whole header in here instead of returning, but this'll work for now
    }*/
	//else{
		//Fputs(header,stdout);      
	Fputs(serverBuffer,stdout);	//TODO: ALL fputs should be changed to write back to client instead of to stdout (but for testing purposes this works)
	while(checkStatus != 0){
		Rio_readlineb(&serverData, serverBuffer, MAXLINE);
		Fputs(serverBuffer,stdout);
		if(strcmp(serverBuffer,"\r\n")== 0){	//how to detect end of header
		  checkStatus = 0;
		  }
		else if(strstr(serverBuffer, "Content-Length:")){
			strcpy(temp,serverBuffer);
			strcpy(temp, strtok(temp, " \n\t\r"));
			strcpy(temp, strtok(NULL, " \n\t\r"));
			bufferSize = atoi(temp);
		}
		//just loop through header, check relevant lines to ensure no errors, and find size of main body
		//then just loop until there's an empty line, then start listing characters in for loop below
	}
	//printf("reading message of: %d bytes\n", bufferSize);
	//}
	int i;
	if(bufferSize > 0){
		for(i = 0; i < bufferSize; i++){
			Rio_readnb(&serverData,serverBuffer,1);
			printf("%c",serverBuffer[0]);
		}
	}
	//TODO: implement logging around here somewhere
	//printf("|||reached end of buffer\n");
    	Close(clientfd); 
    	exit(0); 
} 

