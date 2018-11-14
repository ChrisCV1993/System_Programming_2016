#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include "utils.hpp"
#include <semaphore.h>
#include <pthread.h> 

#define HASHSIZE 101
#define msg_len 256

using namespace std;

pthread_mutex_t mutex_workers = PTHREAD_MUTEX_INITIALIZER;							//lock for worker threads
pthread_mutex_t mutex_master = PTHREAD_MUTEX_INITIALIZER;							//lock for master thread
pthread_cond_t cond_empty = PTHREAD_COND_INITIALIZER;								//cond var if q is empty
pthread_cond_t cond_full = PTHREAD_COND_INITIALIZER;								//cond var if q is full


void *validjob(void * args){														//function job for each worker thread
	Arguments * myargs = (Arguments *) args;									
	Hashtable * hash=myargs->hash;
	Queue * queue=myargs->queue;
	sem_t * sems=myargs->sems;
	while(1){																		
		Node * node;
		pthread_mutex_lock(&mutex_workers);											//lock the workers mutex
		while(queue->get_size()==0){
				pthread_cond_wait(&cond_empty, &mutex_workers);						//check the condition variable
		}
		pthread_mutex_lock(&mutex_master);											//lock the master_mutex for poping
		node=queue->pop();
		pthread_mutex_unlock(&mutex_master);										//unlock the master_mutex for push or pop
		pthread_cond_broadcast(&cond_full);											//signal that the q is not full
		pthread_mutex_unlock(&mutex_workers);										//unlock the workers mutex
		job(node,hash,sems);														//do your job
		delete node;
	}
	pthread_exit(NULL); 
}

int main(int argc,char *argv[]){
	int port;																		
	int thread_pool_size;
	int queue_size;
	int i;
	//------------------------------------------------------Check the arguments---------------------------------//
	if((argc!=7)||(argc%2!=1)){
		cout<<"Wrong number of arguments!"<<endl;
		cout<<"Usage:"<<endl<<"  ./bankserver -p <port> -s <thread_pool_size> -q <queue_size>"<<endl;
		return 1;
	}

	for(i=1;i<6;i+=2){
		if(strcmp(argv[i],"-p")==0){
			port=atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-s")==0){
			thread_pool_size=atoi(argv[i+1]);
		}
		else if(strcmp(argv[i],"-q")==0){
			queue_size=atoi(argv[i+1]);
		}
		else{
			cout<<"Wrong argument!"<<endl;
			return 1;
		}
	}
	if(thread_pool_size==0){
		cout<<"Please enter a valid number of threads"<<endl;
		return 1;
	}
	if(queue_size==0){
		cout<<"Please enter a valid max queue size"<<endl;
		return 1;
	}
	//--------------------------------------------------Up to here---------------------------------------------------//
	//--------------------------------------------------Print the input----------------------------------------------//
	cout<<"Selected port     : "<<port<<endl;
	cout<<"Number of threads : "<<thread_pool_size<<endl;
	cout<<"Max queue size    : "<<queue_size<<endl;
	

	Hashtable * hash;										
	hash=new Hashtable(HASHSIZE);										//create and iniatialize the hash
	sem_t sems[HASHSIZE];												//semaphore for each bucket
	int sem_ret;

	Queue * queue;														

	queue=new Queue;													//create and iniatialize the queue						

	for(i=0;i<HASHSIZE;i++){											//initialize the semaphore array // 0 for shared //
		if((sem_ret=sem_init(&sems[i],0,1))<0){
			perror("Sem intializing error");
		}
	}
	//-------------------------------------------------Prepare the sockets-------------------------------------------//		

	int sock;
	int newsock;
	struct  sockaddr_in  server ;										
	struct  sockaddr_in  client ;
	struct  sockaddr *serverptr =( struct  sockaddr  *)&server;
	struct  sockaddr *clientptr =( struct  sockaddr  *)&client;
	socklen_t  clientlen=sizeof(struct sockaddr_in);
	struct  hostent *rem;

	//------------------------------------------------Prepare the arguments for each thread-------------------------//

	Arguments *args;
	args=new Arguments;
	args->hash=hash;
	args->queue=queue;
	args->sems=sems;

	pthread_t *thr;
	thr=new pthread_t[thread_pool_size];												//create thread_pool_size threads

	for(i=0; i<thread_pool_size; i++){
		pthread_create(&thr[i], NULL, validjob, args);									//they execute the validjob with args arguments
	}

	if ((sock = socket(PF_INET , SOCK_STREAM , 0)) ==  -1)								//create a socket
		perror("Socket  creation  failed!");

	server.sin_family = AF_INET;														//Internet  domain
	server.sin_addr.s_addr = htonl(INADDR_ANY);											//anyone has  
	server.sin_port = htons(port);														//the  given  port  
	
	if (bind(sock , serverptr , sizeof(server)) < 0)									// Bind  socket  to  address  
		perror("bind");
	if (listen(sock , 10) < 0)  														// listen for connections
		perror("listen");
	printf("Listening for connections to port %d\n", port);

	//-------------------------------------------------------------Master thread-----------------------------------------------//

	while  (1) {
		/*  accept  connection  */
		if (( newsock = accept(sock , clientptr , &clientlen)) < 0)  					//accept a connection
			perror("accept");
		pthread_mutex_lock(&mutex_master);												//lock the master mutex
		while(queue->get_size()==queue_size){		
			pthread_cond_wait(&cond_full, &mutex_master);								//check if full
		}
		queue->push_back(newsock,client,clientptr,clientlen);							//push back the new socket
		pthread_cond_broadcast(&cond_empty);											//signal that the q is not empty
		pthread_mutex_unlock(&mutex_master);											//unlock the master mutex
	}
	//----------------------------------------------------------Deletions-------------------------------------------------------//
    delete hash;
    for(i=0;i<HASHSIZE;i++){
		sem_destroy(&sems[i]);
	}
	close(sock);
	for(i=0; i<thread_pool_size; i++){
		pthread_join(thr[i], NULL);
	}
	delete[] thr;
	delete args;
	//cout<<"Socket close"<<endl;
	return 0;
}