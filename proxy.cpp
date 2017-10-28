#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include<errno.h>
#include<string>
#define BACKLOG 1     // how many pending connections queue will hold
using namespace std;

int main(int argc,char* argv[]) {
	int sockfd, new_fd,valread;  // listen on sock_fd, new connection on new_fd

	struct sockaddr_in serveraddr;
	struct sockaddr_in hostaddr;
	struct hostent* host;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char buffer[1024] = {0};
	int newsockfd,newclisockfd,port=80,rv=0;

	//server addr details
	serveraddr.sin_family = AF_UNSPEC;
	serveraddr.sin_port = htons(atoi(argv[1]));
	serveraddr.sin_addr.s_addr = INADDR_ANY;

	if ((sockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1) {
		perror("server: socket");
	}
	unsigned int size =sizeof(serveraddr);
	if (bind(sockfd, (struct sockaddr*)&serveraddr,size) == -1) {
		close(sockfd);
		perror("server: bind");
	}
	//listening for client request
	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}

	while(1) {
		sin_size = sizeof their_addr;
		//accepts the client socket
		new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
		if (new_fd == -1) {
			perror("accept");
			exit(1);
		}
		if (!fork()) { // this is the child process
			bzero((char*)buffer,1024);
			char method[500],address[500],protocol[10];
			string path="";
			valread = recv( new_fd , buffer, 1024, 0);
			sscanf(buffer,"%s %s %s",method,address,protocol);
			printf("%s %s %s",method,address,protocol);
              //validating client request
			if(((strncmp(method,"GET",3)==0))&&((strncmp(protocol,"HTTP/1.0",8)==0)||(strncmp(protocol,"HTTP/1.1",8)==0))&&(strncmp(address,"http://",7)==0))
			{
				//parsing the address
				std::string addressString = std::string(address);
				std::string hostname;
				hostname = addressString.substr(7);
				int pathLocation = hostname.find_first_of('/');
				if(pathLocation > 0) {
					//parsing the path name hostname
					path = hostname.substr(pathLocation+1);
					hostname = hostname.substr(0,pathLocation);
				}
				bzero((char*)&hostaddr,sizeof(hostaddr));
				hostaddr.sin_port=htons(port);
				hostaddr.sin_family=AF_INET;
                //gets the hostname after parsing the client request
				host=gethostbyname(hostname.c_str());
				bcopy((char*)host->h_addr,(char*)&hostaddr.sin_addr.s_addr,host->h_length);
				if ((newsockfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == -1) {
					perror("server: socket");
				}
				//connecting to actual web server
				if((newclisockfd=connect(newsockfd,(struct sockaddr*)&hostaddr,sizeof(struct sockaddr))) < 0) {
					perror("server connection failed.");
				}
				sprintf(buffer,"\nConnected to %s  IP - %s\n",hostname.c_str(),inet_ntoa(hostaddr.sin_addr));
				printf("\n%s\n",buffer);
				bzero((char*)buffer,sizeof(buffer));
				//recontructing the request in the form of http get header and body
				if(path.size()!=0) {
					sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nAccept: text/html\r\nAccept-Language: en-us\r\nConnection: close\r\n\r\n",path.c_str(),protocol,hostname.c_str());
					printf("\n%s\n",buffer);
				}
				else {
					sprintf(buffer,"GET / %s\r\nHost: %s\r\nAccept: text/html\r\nAccept-Language: en-us\r\nConnection: close\r\n\r\n",protocol,hostname.c_str());
					printf("\n%s\n",buffer);
				}
				//sending the received response to client
				if((rv=send(newsockfd,buffer,strlen(buffer),0)) < 0)
				{
					perror("Error writing to socket");
				}
				else {

					do
					{
						bzero((char*)buffer,1024);
						rv=recv(newsockfd,buffer,1024,0);
						if(!(rv<=0))
							send(new_fd,buffer,rv,0);
					}while(rv>0);
				}
			}
			else
			{
				//sends 500 bad request to client. if the client request is not in expected format
				send(new_fd,"500 : BAD REQUEST",18,0);
			}
		}

	}
	close(sockfd);
	close(new_fd);
	close(newsockfd);
	close(newclisockfd);

}
