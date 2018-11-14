#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

//#include "utils.hpp"

#define string_len 512


using namespace std;


int main(int argc,char *argv[]){
	int i;
	int server_port;
	char * server_name;
	char * filename;

	//------------------------------------------------------Check the arguments---------------------------------//
	if((argc!=7)||(argc%2!=1)){
		cout<<"Wrong number of arguments!"<<endl;
		cout<<"Usage:"<<endl<<"  ./bankclient -h <server host> -p <server port> -i <command file>"<<endl;
		return 1;
	}

	for(i=1;i<6;i+=2){
		if(strcmp(argv[i],"-h")==0){
			server_name=new char[strlen(argv[i+1])];
			strncpy(server_name,argv[i+1],strlen(argv[i+1]));
		}
		else if(strcmp(argv[i],"-p")==0){
			server_port=atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-i")==0){
			filename=new char[strlen(argv[i+1])];
			strncpy(filename,argv[i+1],strlen(argv[i+1]));
		}
		else{
			cout<<"Wrong argument!"<<endl;
			return 1;
		}
	}

	//------------------------------------------------------Sockets---------------------------------//
	cout<<"Server host         : "<<server_name<<endl;
	cout<<"Server port         : "<<server_port<<endl;
	cout<<"Command file name   : "<<filename<<endl;
	
	int sock;
	struct  sockaddr_in  server;
	struct  sockaddr *serverptr = (struct  sockaddr *)&server;
	struct  hostent *rem;

													
	if ((sock = socket(PF_INET , SOCK_STREAM , 0)) < 0)								//create socket
		perror("socket");
	if ((rem = gethostbyname(server_name)) == NULL) {								//find server address
		herror("gethostbyname"); 
		return 1;
	}
	

	server.sin_family = AF_INET;													//internet domain
	memcpy (& server.sin_addr , rem ->h_addr , rem ->h_length);
	server.sin_port = htons(server_port);											// Server  port  
	
	
	if (connect(sock , serverptr , sizeof(server)) < 0){							// Initiate  connection  
		perror("connect");
	}
	printf("Connecting  to %s port %d\n", server_name, server_port);

	FILE * fptr;
	char message[string_len];
	if (strncmp(filename,"0",strlen(filename))!=0){
		fptr=fopen(filename,"r+");
		if(fptr==NULL){
			cout<<"Cannot open the specified file"<<endl;
			return 1;
		}
	}
	else
		fptr=stdin;
	int check_send=0;
	int check_recv=0;
	char reply[string_len];
	socklen_t  serverlen=sizeof(server);
	bzero(reply,string_len);
	while(1){
		i=0;
		if (fgets(message,string_len,fptr) <= 0)										//read line by line until EOF is given
			break; 
		if(strncmp(message,"exit",4)==0)												//check if it is exit
			break;
		if(strncmp(message,"sleep",5)==0){												//check if it is sleep
			int ret_scan=0;
			int milsecs;
			ret_scan=sscanf(message+6,"%d",&milsecs);
			if(ret_scan<0){
				cout<<"error in sscanf"<<endl<<"Terminating..."<<endl;
			}
			milsecs=milsecs*1000;
			usleep(milsecs);
			continue;
		}
		if((check_send=sendto(sock,message,strlen(message)+1,0, serverptr , sizeof(server)))<0)			//send message to server
			perror("check_send");
		if((check_recv=recvfrom(sock,reply ,(unsigned int)sizeof(reply) ,0 , serverptr , &serverlen))<0) 	//receive reply from server
			perror("check_recv");
		cout<<reply<<endl;																				//print the reply
		bzero(reply,sizeof(reply));
	}
	close(sock);
	cout<<"Closing socket..."<<endl;
	fclose(fptr);
	delete[] filename;
	delete[] server_name;

}