
#include "utils.hpp"

#define msg_len 512

using namespace std;

//--------------------------------------------Q Node Functions-----------------------------------//
Node::Node(int val,struct  sockaddr_in clt,struct  sockaddr * cltptr,socklen_t len){
	data=val;
	client=clt;
	clientptr=cltptr;
	clientlen=len;
	//cout<<"A new Node has just been created"<<endl;
}

Node::~Node(){
	//cout<<"An Node hash just been destroyed"<<endl;
}

void Node::set_data(int dat){
	data=dat;
}
		
int Node::get_data(){
	return data;
}

struct  sockaddr_in Node::get_client(){
	return client;
}

struct  sockaddr * Node::get_clientptr(){
	return clientptr;
}

socklen_t Node::get_clientlen(){
	return clientlen;
}

void Node::set_next(Node* temp){
	next=temp;
}

Node* Node::get_next(){
	return next;
}

void Node::print_data(){
	cout<<"Data : "<<data<<endl;
}

//--------------------------------------------Queue Functions-----------------------------------//

Queue::Queue(){
	head=NULL;
	tail=NULL;
	size=0;
	cout<<"A new Queue hash just been created"<<endl;
}

Queue::~Queue(){
	Node * temp;
	while(head!=NULL){
		temp=pop();
		delete temp;
	}
	cout<<"A Queue has just been destroyed"<<endl;
}
int Queue::get_size(){
	return size;
}

Node * Queue::pop(){
	Node * temp;
	if(size==0)
		return NULL;
	temp=head;
	head=temp->get_next();
	size--;
	return temp;
}

void Queue::push_back(int val,struct  sockaddr_in client,struct  sockaddr * clientptr,socklen_t clientlen){
	Node * temp;
	temp=new Node(val,client,clientptr,clientlen);
	if(size==0){
		head=temp;
		tail=temp;
	}
	else{
		tail->set_next(temp);
		tail=temp;
	}
	size++;
}

void Queue::print_list(){
	Node * temp;
	temp=head;
	while(temp!=NULL){
		temp->print_data();
		temp=temp->get_next();
	}
}

//-------------------------------------------------------------Quicksort-----------------------------------//

void qsort(int *a, int lo, int hi) {
  int h, l, p, t;
  if (lo < hi) {
    l = lo;
    h = hi;
    p = a[hi];
    do {
      while ((l < h) && (a[l] <= p)) 
          l = l+1;
      while ((h > l) && (a[h] >= p))
          h = h-1;
      if (l < h) {
          t = a[l];
          a[l] = a[h];
          a[h] = t;
      }
    } while (l < h);
    a[hi] = a[l];
    a[l] = p;
    qsort( a, lo, l-1 );
    qsort( a, l+1, hi );
  }
}

//------------------------------------------------------------remove duplicates-----------------------------------//

void no_doubles(int *a,int lo,int hi){
	int i;
	int temp;
	temp=a[lo];
	for(i=lo+1;i<hi;i++){
		if(a[i]==temp)
			a[i]=-1;
		else
			temp=a[i];
	}
}

int identify(char * reply,Hashtable * hash,char * answer,sem_t * sems){
	
	if(strncmp(reply,"add_account",11)==0){										//ADD ACCOUNT 
		int ret_scan=0;
		int val;
		char name[64];
		int milsecs=0;
		int ret_hash;
		AccNode *node_source=NULL;
		int pos;
		int sem_ret=-1;
		bzero(answer,sizeof(answer));
		if((ret_scan=sscanf(reply+11,"%d %s %d",&val,name,&milsecs))<0){		//get the value the name and the [delay]
			cout<<"ret_scan error"<<endl;
			return 1;
		}
		
		pos=hash->hash_function(name);											//find the specific bucket

		while(sem_ret<0){														//try to get the lock 
			sem_ret=sem_trywait(&sems[pos]);
		}
		//------------------------------------------------Critical Section----------------//
		milsecs=milsecs*1000;
		if(milsecs!=0){
			usleep(milsecs);
		}
		milsecs=milsecs/1000;

		node_source=hash->lookupnode(name);
		if(node_source!=NULL){
			if(milsecs!=0)
				sprintf(answer,"Error. Account creation failed (%s:%d:%d)",name,val,milsecs);
			else
				sprintf(answer,"Error. Account creation failed (%s:%d)",name,val);
			sem_post(&sems[pos]);
			return 0;
		}
		if((ret_hash=hash->insert_hash(name,val))==0){
			if(milsecs!=0)
				sprintf(answer,"Success. Account creation (%s:%d:%d)",name,val,milsecs);
			else
				sprintf(answer,"Success. Account creation (%s:%d)",name,val);
		}
		else{
			if(milsecs!=0)
				sprintf(answer,"Error. Account creation failed (%s:%d:%d)",name,val,milsecs);
			else
				sprintf(answer,"Error. Account creation failed (%s:%d)",name,val);
		}
		//------------------------------------------------End of critical section---------//
		sem_post(&sems[pos]);														//unlock
		return 0;
	}
	else if((strncmp(reply,"add_transfer",12))==0){									//ADD TRANSFER
		int ret_scan=0;
		int val;
		char source[64];
		char dest[64];
		int milsecs=0;
		int ret_hash;
		AccNode *node_source=NULL;
		AccNode *node_dest=NULL;
		int balance_s;
		int balance_d;
		int balance_tr;
		TrList * list;
		TrNode * tr_node=NULL;
		int pos[2];
		int sem_ret=-1;
		bzero(answer,sizeof(answer));
		if((ret_scan=sscanf(reply+13,"%d %s %s %d",&val,source,dest,&milsecs))<0){ 			//initialize  the vars
			cout<<"ret_scan error"<<endl;
			return 1;
		}
		pos[0]=hash->hash_function(source);												//find 1st bucket
		pos[1]=hash->hash_function(dest);												//find 2nd bucket
		qsort(pos,0,1);																	//sort the pos array
		no_doubles(pos,0,2);															//remove the duplicates
		while(sem_ret<0){
			sem_ret=sem_trywait(&sems[pos[0]]);											//get 1st lock 
		}
		sem_ret=-1;
		if(pos[1]!=-1){																	//get second if needed
			while(sem_ret<0){
			sem_ret=sem_trywait(&sems[pos[1]]);
			}
		}
		//------------------------------------------------Critical Section----------------//
		milsecs=milsecs*1000;
		if(milsecs!=0){
			usleep(milsecs);
		}
		milsecs=milsecs/1000;

		node_source=hash->lookupnode(source);											//find the nodes
		node_dest=hash->lookupnode(dest);
		if((node_source==NULL)||(node_dest==NULL)){
			if(milsecs==0)
				sprintf(answer,"Error. Transfer addition failed (%d:%s:%s) Reason: Node does not exist",val,source,dest);
			else
				sprintf(answer,"Error. Transfer addition failed (%d:%s:%s:%d) Reason: Node does not exist",val,source,dest,milsecs);
			sem_post(&sems[pos[0]]);
			if(pos[1]!=-1)
				sem_post(&sems[pos[1]]);
			return 1;
		}
		balance_s = node_source->get_value();											//check if balance is sufficient
		if(balance_s<val){
			if(milsecs==0)
				sprintf(answer,"Error. Transfer addition failed (%d:%s:%s) Reason: Insufficient balance",val,source,dest);
			else
				sprintf(answer,"Error. Transfer addition failed (%d:%s:%s:%d) Reason: Insufficient balance",val,source,dest,milsecs);
			sem_post(&sems[pos[0]]);
			if(pos[1]!=-1)
				sem_post(&sems[pos[1]]);
			return 1;
		}
		balance_s=balance_s-val;														//fix the list 
		node_source->set_value(balance_s);
		balance_d=node_dest->get_value();
		balance_d=balance_d+val;
		node_dest->set_value(balance_d);
		list=node_dest->get_transfers();
		if(list==NULL){
			node_dest->insert_transfer(source,val);
		}
		else{
			tr_node=list->exists2(source);
			if(tr_node==NULL){
				node_dest->insert_transfer(source,val);
			}
			else{
				balance_tr=tr_node->get_value();
				balance_tr=balance_tr+val;
				tr_node->set_value(balance_tr);
			}
		}
		if(milsecs==0)
			sprintf(answer,"Success. Transfer addition (%d:%s:%s)",val,source,dest);
		else
			sprintf(answer,"Success. Transfer addition (%d:%s:%s:%d)",val,source,dest,milsecs);
		//------------------------------------------------End of critical section---------//
		sem_post(&sems[pos[0]]);
		if(pos[1]!=-1)
			sem_post(&sems[pos[1]]);
		return 0;
	}
	else if((strncmp(reply,"print_balance",13))==0){										//PRINT BALANCE
		int ret_scan=0;
		char name[64];
		int ret_hash;
		AccNode *node_source=NULL;
		int balance;
		int milsecs=0;
		int pos;
		int sem_ret=-1;
		bzero(answer,sizeof(answer));
		if((ret_scan=sscanf(reply+14,"%s %d",name,&milsecs))<0){
			cout<<"ret_scan error"<<endl;
			return 1;
		}
		pos=hash->hash_function(name);														///get the lock
		while(sem_ret<0){
			sem_ret=sem_trywait(&sems[pos]);
		}
		//------------------------------------------------Critical Section----------------//
		milsecs=milsecs*1000;
		if(milsecs!=0){
			usleep(milsecs);
		}
		milsecs=milsecs/1000;

		node_source=hash->lookupnode(name);
		if(node_source==NULL){
			if(milsecs==0)
				sprintf(answer,"Error. Balance (%s) Reason: Node does not exist",name);
				
			else
				sprintf(answer,"Error. Balance (%s:%d) Reason: Node does not exist",name,milsecs);
			sem_post(&sems[pos]);
			return 1;
		}
		balance=node_source->get_value();
		if(milsecs==0)
			sprintf(answer,"Success. Balance (%s:%d)",name,balance);
		else
			sprintf(answer,"Success. Balance (%s:%d:%d)",name,balance,milsecs);
		//------------------------------------------------End of critical section---------//
		sem_post(&sems[pos]);
		return 0;
	}
	else if((strncmp(reply,"add_multi_transfer",18))==0){
		int ret_scan=0;
		char ** names;
		int mult=0;
		int max=0;
		char temp_ch[64];
		int ret_hash;
		AccNode *node_source=NULL;
		AccNode *temp=NULL;
		TrList * list;
		TrNode * tr_node=NULL;
		int balance;
		int balance_s;
		int milsecs=0;
		bzero(answer,sizeof(answer));
		int pointer=19;
		int counter=0;
		int i;
		int j=0;
		int flag=0;
		int *pos;
		int sem_ret=-1;
		while((ret_scan=sscanf(reply+pointer,"%s",temp_ch))>0){											//find all names
			pointer=pointer+1+strlen(temp_ch);
			counter++;
		}
		names=new char*[counter];
		for(i=0;i<counter;i++){
			names[i]=new char[32];
		}
		i=0;
		pointer=19;
		while((ret_scan=sscanf(reply+pointer,"%s",names[i]))>0){
			pointer=pointer+1+strlen(names[i]);
			i++;
		}
		balance=atoi(names[0]);
		for(j=0;j<(strlen(names[counter-1]));j++){
			if(!isdigit(names[counter-1][j])){
				flag=1;
			 	break;
			}
		}
		mult=counter-2;
		max=counter;
		if(flag==0){
			milsecs=atoi(names[counter-1]);
			mult=counter-3;
			max=counter-1;
		}
		pos=new int[max];
		for(i=0;i<max;i++){																	//find all locks
			pos[i]=hash->hash_function(names[i]);
		}
		qsort(pos,0,max-1);																	//sort them
		no_doubles(pos,0,max);
		for(i=0;i<max;i++){																	//lock those needed
			if(pos[i]!=-1){
				while(sem_ret<0){
					sem_ret=sem_trywait(&sems[pos[i]]);
				}
			}
			sem_ret=-1;
		}
		//------------------------------------------------Critical Section----------------//
		milsecs=milsecs*1000;
		if(milsecs!=0){
			usleep(milsecs);
		}
		milsecs=milsecs/1000;

		node_source=hash->lookupnode(names[1]);											//check the source
		if(node_source==NULL){
			sprintf(answer,"Error. Multi-Transfer addition failed (%d:%s",balance,names[1]);
			for(i=2;i<counter;i++){
				cout<<strlen(answer)<<endl;
				strcat(answer,":");
				strcat(answer,names[i]);
			}
			strcat(answer,") Reason Node does not exist");
			for(i=0;i<counter;i++){
				delete[] names[i];
			}
			delete[] names;
			for(i=0;i<max;i++){
				if(pos[i]!=-1)
					sem_post(&sems[pos[i]]);
			}
			delete[] pos;
			return 1;	
		}
		balance_s=node_source->get_value();												//get the balance
		if(balance_s<(mult*balance)){
			sprintf(answer,"Error. Multi-Transfer addition failed (%d:%s",balance,names[1]);
			for(i=2;i<counter;i++){
				strcat(answer,":");
				strcat(answer,names[i]);
			}
			strcat(answer,") Reason: Insufficient balance");
			for(i=0;i<counter;i++){
				delete[] names[i];
			}
			delete[] names;
			for(i=0;i<max;i++){
				if(pos[i]!=-1)
					sem_post(&sems[pos[i]]);
			}
			delete[] pos;
			return 1;
		}
		node_source->set_value(balance_s-mult*balance);									//fix all the other nodes
		for(i=2;i<max;i++){
			temp=NULL;
			temp=hash->lookupnode(names[i]);
			if(temp==NULL){
				sprintf(answer,"Error. Multi-Transfer addition failed (%d:%s",balance,names[1]);
				for(j=2;j<counter;j++){
					strcat(answer,":");
					strcat(answer,names[j]);
				}
				strcat(answer,") Reason Node does not exist");
				for(j=0;j<counter;j++){
					delete[] names[j];
				}
				delete[] names;
				for(j=0;j<max;j++){
					if(pos[j]!=-1)
						sem_post(&sems[pos[j]]);
				}
				delete[] pos;
				return 1;	
			}
			balance_s=temp->get_value();
			temp->set_value(balance+balance_s);
			list=temp->get_transfers();
			if(list==NULL){
				temp->insert_transfer(names[1],balance);
			}
			else{
				tr_node=list->exists2(names[1]);
				if(tr_node==NULL){
					temp->insert_transfer(names[1],balance);
				}
				else{
					balance_s=tr_node->get_value();
					balance_s=balance_s+balance;
					tr_node->set_value(balance_s);
				}
			}
		}
		sprintf(answer,"Success. Multi-Transfer addition  (%d:%s",balance,names[1]);
		for(i=2;i<counter;i++){
			strcat(answer,":");
			strcat(answer,names[i]);
		}
		strcat(answer,")");

		for(i=0;i<counter;i++){
			delete[] names[i];
		}
		delete[] names;
		//------------------------------------------------End of critical section---------//
		for(j=0;j<max;j++){														//unlock everything 
			if(pos[j]!=-1)
				sem_post(&sems[pos[j]]);
		}
		delete[] pos;
		return 0;
	}
	else if((strncmp(reply,"print_multi_balance",19))==0){
		int ret_scan=0;
		char ** names;
		int * balances;
		int mult=0;
		int max=0;
		char name[64];
		char numb[32];
		char temp_ch[64];
		int ret_hash;
		AccNode *node_source=NULL;
		AccNode *temp=NULL;
		TrList * list;
		TrNode * tr_node=NULL;
		int balance;
		int milsecs=0;
		bzero(answer,sizeof(answer));
		int pointer;
		pointer=20;
		int counter=0;
		int i;
		int j=0;
		int *pos;
		int sem_ret=-1;
		while((ret_scan=sscanf(reply+pointer,"%s",temp_ch))>0){					//find all the names
			pointer=pointer+1+strlen(temp_ch);
			counter++;
		}
		names=new char*[counter];
		balances=new int[counter];
		for(i=0;i<counter;i++){
			names[i]=new char[32];
			balances[i]=0;
		}
		i=0;
		pointer=20;
		while((ret_scan=sscanf(reply+pointer,"%s",names[i]))>0){
			pointer=pointer+1+strlen(names[i]);
			i++;
		}
		int flag=0;
		for(j=0;j<(strlen(names[counter-1]));j++){
			if(!isdigit(names[counter-1][j])){
				flag=1;
			 	break;
			}
		}
		max=counter;
		if(flag==0){
			milsecs=atoi(names[counter-1]);
			max=counter-1;
		}
		pos= new int[max];
		for(i=0;i<max;i++){
			pos[i]=hash->hash_function(names[i]);									//find all teh locks
		}
		qsort(pos,0,max-1);															//sort them
		no_doubles(pos,0,max);
		for(i=0;i<max;i++){
			if(pos[i]!=-1){
				while(sem_ret<0){
					sem_ret=sem_trywait(&sems[pos[i]]);
				}
			}
			sem_ret=-1;
		}
		//------------------------------------------------Critical Section----------------//
		milsecs=milsecs*1000;
		if(milsecs!=0){
			usleep(milsecs);
		}
		milsecs=milsecs/1000;

		for(i=0;i<max;i++){
			temp=hash->lookupnode(names[i]);
			if(temp==NULL){
				sprintf(answer,"Error. Multi-Balance failed (%s",names[0]);
				for(j=1;j<counter;j++){
					strcat(answer,":");
					strcat(answer,names[j]);
				}
				strcat(answer,") Reason : Node does not exist");
				for(j=0;j<counter;j++){
					delete[] names[j];
				}
				delete[] names;
				delete[] balances;
				for(j=0;j<max;j++){
					if(pos[j]!=-1)
						sem_post(&sems[pos[j]]);
				}
				delete[] pos;
				return 1;
			}
			balances[i]=temp->get_value();
		}
		sprintf(answer,"Success. Multi-Balance (%s/%d",names[0],balances[0]);
		for(j=1;j<max;j++){
			sprintf(answer+strlen(answer),":%s/%d",names[j],balances[j]);
		}
		if(milsecs==0){
			strcat(answer,")");
		}
		else{
			strcat(answer,":");
			strcat(answer,names[counter-1]);
			strcat(answer,")");
		}	
		
		for(j=0;j<counter;j++){
			delete[] names[j];
		}
		delete[] names;
		delete[] balances;
		for(j=0;j<max;j++){
			if(pos[j]!=-1)
				sem_post(&sems[pos[j]]);
		}
		delete[] pos;
		return 0;
	}
	else
		strncpy(answer,"Error. Unknown command",22);
	return 0;
}

void job(Node* node,Hashtable* hash,sem_t* sems){
	int newsock;
	struct  sockaddr_in  client ;
	struct  sockaddr *clientptr =( struct  sockaddr  *)&client;
	socklen_t  clientlen=sizeof(struct sockaddr_in);
	struct  hostent *rem;
	client=node->get_client();
	clientptr=node->get_clientptr();
	clientlen=node->get_clientlen();
	newsock=node->get_data();
	
	/*  Find  client 's  address  */

  	if  (( rem =  gethostbyaddr (( char  *) &client.sin_addr.s_addr ,  sizeof(client.sin_addr.s_addr), client.sin_family )) ==  NULL) {
        	herror (" gethostbyaddr "); 
    return ;}
       
    printf (" Accepted  connection  from %s\n", rem ->h_name);
	printf("Accepted  connection\n");
		
	int check_send=0;
	int check_recv=0;
	char reply[msg_len];
	int ident;
	//char copy_rep[256];
	char message[msg_len];
	bzero(message,sizeof(message));
	strcpy(message,"ok");
	
	while(1){
		if((check_recv=recvfrom(newsock,reply ,(unsigned int)sizeof(reply) ,0 , clientptr , &clientlen))<=0){		//get the message
			//cout<<"check_recv"<<endl;
			break;
		}
		ident=identify(reply,hash,message,sems);																	//idetify it
		bzero(reply,sizeof(reply));
		if((check_send=sendto(newsock,message,strlen(message),0, clientptr , sizeof(client)))<0){
			cout<<"check_send"<<endl;
			break;
		}
		bzero(message,sizeof(message));
	}
	return ;
}


