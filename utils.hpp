#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include "hashtable.hpp"
#include <semaphore.h>
#include <pthread.h> 

#ifndef UTILS_HPP
	#define UTILS_HPP

using namespace std;



class Node{												//queue node 
	int data;
	struct  sockaddr_in client;
	struct  sockaddr * clientptr;
	socklen_t clientlen;
	Node * next;

	public:
		Node(int,struct  sockaddr_in,struct  sockaddr *,socklen_t);
		~Node();
		void set_data(int);
		int get_data();
		void set_next(Node*);
		struct  sockaddr_in get_client();
		struct  sockaddr * get_clientptr();
		socklen_t get_clientlen();
		Node* get_next();
		void print_data();
};


class Queue{											//list with only pop and push_back
	Node * head;
	Node * tail;
	int size;

	public:
		Queue();
		~Queue();
		Node * pop();
		int get_size();
		void push_back(int,struct  sockaddr_in,struct  sockaddr *,socklen_t);
		void print_list();
};

typedef struct Arguments_t{								//argument struct for passing arguments to thread job
	Hashtable * hash;
	Queue * queue;
	sem_t * sems;
}Arguments;

void qsort(int *,int,int);								//quicksort

void no_doubles(int *,int,int);							//remove the duplicates from a sorted array

int identify(char *,Hashtable *,char*,sem_t *);			//check which message you received
	
void job(Node *,Hashtable*,sem_t*);						//thread job


#endif