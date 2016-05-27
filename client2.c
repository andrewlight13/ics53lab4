#include "csapp.h" 

/* usage: ./echoclient host port */
int main(int argc, char **argv)
{ 
	//"server" stuff referrs to connecting to webserver, "browser" variables refer to connecting to user's browser, anything else is either local proxy data or i've forgotten to change the name
	int serverfd, port, browserfd, browserreadfd, clientlen; 
	struct sockaddr_in clientaddr;
	char *host, serverBuffer[MAXLINE], header[MAXLINE];	//header not necessary if commented out if/else isn't needed, delete if that's the case 
	rio_t serverData;
	rio_t browserData; 
	host = argv[1];  port = atoi(argv[2]); 
	while(1){
		serverfd = Open_clientfd(host, argv[2]);
		browserfd = Open_listenfd(argv[3]);
		browserreadfd = Accept(browserfd, (SA *)&clientaddr, &clientlen);	//then just do stuff with clientaddr for logging, see main.c for info on that
		Rio_readinitb(&serverData, serverfd);
		Rio_readinitb(&browserData, browserreadfd);
		printf(":"); fflush(stdout);
		int checkStatus = 1;
		while (checkStatus!=0) {
			Rio_readlineb(&browserData, serverBuffer, MAXLINE);	//GET request from browser
			Fputs(serverBuffer, stdout);
			Rio_writen(serverfd, serverBuffer, strlen(serverBuffer));  //sends data to server, get rid of placeholder code above this
			if(strcmp(serverBuffer, "\r\n") == 0) break;	//when end of header detected, break and send data to server
			//if(strcmp(serverBuffer, "\r\n") == 0) break;
		}
		//check first line for 200 OK and HTTP version
		//ok not sure this is neccessary, so commented it out
		//Rio_readlineb(&serverData, header,MAXLINE);
		Rio_readlineb(&serverData,serverBuffer,MAXLINE);
		checkStatus = 1;
		int bufferSize;
		char temp[MAXLINE];
		/*if(strstr(header,"HTTP/1.0 200 OK") == 0 && strstr(header,"HTTP/1.1 200 OK") == 0){	//this probably isn't necessary, but if we need to detect HTTP errors proxyside, uncomment this and update
			Fputs(header,stdout);																//but we SHOULD be able to just pass the error data back
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
			 
			//Close(serverfd);
			//return;        //ideally, just print the whole header in here instead of returning, but this'll work for now
	    }*/
		//else{
			//Fputs(header,stdout);      
		Fputs(serverBuffer,stdout);	//TODO: ALL fputs should be changed to write back to client instead of to stdout (but for testing purposes this works)
		while(checkStatus != 0){
			Rio_readlineb(&serverData, serverBuffer, MAXLINE);	//RESPONSE message
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
				//printf("%c",serverBuffer[0]);
				Rio_writen(browserreadfd, serverBuffer, 1);
			}
		}
		Rio_writen(browserreadfd, "\r\n", 2);
		printf("wrote %d bytes to client\n", bufferSize);
		//TODO: implement logging around here somewhere
		//printf("|||reached end of buffer\n");
		/*Rio_readlineb(&browserData,serverBuffer, MAXLINE);
		if(strlen(serverBuffer) > 0){	//ok, so this DOESNT work, but basically
			printf("serverbuffer now |%s|\n", serverBuffer);	//browser sockets are getting closed too quickly, requests for images are getting lost
		}*/
		Close(serverfd); 
		Close(browserfd);
		Close(browserreadfd);
	}
    	exit(0); 
} 

