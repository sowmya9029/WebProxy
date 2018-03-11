#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<errno.h>
#include <iostream>
#include<string>
#include<string.h>
#define BUFFERSIZE 8192
#define BACKLOG 100     // how many pending connections queue will hold
using namespace std;

void *callServer( void *ptr );
void writeToclientSocket(const char* buff_to_server,int sockfd,int buff_length)
{
	string temp;
	temp.append(buff_to_server);
	int totalsent = 0;
	int senteach;

	while (totalsent < buff_length) {
		if ((senteach = send(sockfd, (void *) (buff_to_server + totalsent), buff_length - totalsent, 0)) < 0) {
			fprintf (stderr," Error in sending to server ! \n");
			exit (1);
		}
		totalsent += senteach;
	}
}
void writeToClient (int Clientfd, int Serverfd) {
	int MAX_BUF_SIZE = 5000;

	int iRecv;
	char buf[MAX_BUF_SIZE];

	while ((iRecv = recv(Serverfd, buf, MAX_BUF_SIZE, 0)) > 0) {
		writeToclientSocket(buf, Clientfd,iRecv);         // writing to client
		memset(buf,0,sizeof buf);
	}

	/* Error handling */
	if (iRecv < 0) {
		fprintf (stderr,"Yo..!! Error while recieving from server ! \n");
		exit (1);
	}
}

void *callServer(void *socket_desc)
{

	struct sockaddr_in hostaddr;
	struct hostent* host;
	char *buffer = (char*)malloc(BUFFERSIZE);
	int newsockfd=0,newclisockfd=0,port=80,rv=0;
	unsigned int new_fd=*(int*)socket_desc;
	bzero((char*)buffer,BUFFERSIZE);
	char method[500],address[500],protocol[10];
	std::string path="";
	recv(new_fd , buffer, BUFFERSIZE, 0);
	sscanf(buffer,"%s %s %s",method,address,protocol);
	printf("%s %s %s",method,address,protocol);

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
		//recontructing the request in the form of http get header and body
		if(path.size()!=0) {

			sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nAccept:text/html,image/*,application/*Accept-Charset:ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\nAccept-Language: en-us\r\nAccept-Encoding:gzip,deflate\r\n"
					"User-Agent:Mozilla/5.0\r\nConnection: close\r\n\r\n",path.c_str(),protocol,hostname.c_str());
			printf("\n%s\n",buffer);
		}
		else {
			sprintf(buffer,"GET / %s\r\nHost: %s\r\nAccept:text/html,image/*,application/*\r\nAccept-Charset:ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\nAccept-Language: en-us\r\n"
					"Accept-Encoding:gzip,deflate\r\nUser-Agent: Mozilla/5.0\r\nConnection: close\r\n\r\n"
					,protocol,hostname.c_str());
			printf("\n%s\n",buffer);
		}

		if((rv=send(newsockfd,buffer,strlen(buffer),0)) < 0)
		{
			perror("Error writing to socket");
		}
		else {
			char buf[BUFFERSIZE];

			while((rv=recv(newsockfd,buf,BUFFERSIZE,0))>0) {

				send(new_fd,buf,rv,0);
				memset(buf,0,sizeof buf);
			}
			//pthread_exit(NULL);
		}
	}
	else
	{
		send(new_fd,"500 : BAD REQUEST",18,0);
	}


	close(newsockfd);
	close(newclisockfd);
	close(new_fd);
	return 0;
}

int main(int argc,char* argv[]) {
	//check for port number
	if(argc<2)
	{
		perror("no port number");
		return -1;
	}

	int sockfd;
	struct sockaddr_in server_adr, client_adr;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	pthread_t *threads = new pthread_t[BACKLOG];
	if(sockfd < 0)
	{
		std::cout << "Something strange happened... exiting!!" << std::endl;
		perror("failed to create socket");
	}

	// char* sin_zero field must be set to 0
	memset((char *)&server_adr, 0, sizeof(server_adr));

	server_adr.sin_family = AF_INET;
	// not making server bind to a particular ip
	server_adr.sin_addr.s_addr = INADDR_ANY;
	server_adr.sin_port = htons(atoi(argv[1]));

	// bind is like providing name to the socket which was created above.
	int tempfd = bind(sockfd, (struct sockaddr *)&server_adr, sizeof(server_adr));
	if(tempfd < 0) {
		cout << "Error in binding socket..." << endl;
		exit(1);
	}


	// only applicable to sockets created using SOCK_STREAM ie
	// for connection-based sockets types.
	listen(sockfd, BACKLOG);
	int newsockfd, count = 0;
	while(1) {
		socklen_t clilen = sizeof client_adr;
		newsockfd = accept(sockfd, (struct sockaddr*)& client_adr, &clilen);
		if(newsockfd < 0) {
			cout << "Error accepting connections" << endl;
		}

		//pthread for multiple client requests
		if(pthread_create(&(threads[count]), NULL, callServer, (void*)&newsockfd) < 0) {
			cout << "Error creating thread.. exiting" << endl;
			exit(1);
		}
		pthread_detach(threads[count]);

		count++;
	}

	return 0;
}
