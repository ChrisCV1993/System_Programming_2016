#include <iostream>
#include <cstring>
#include "lista.hpp"

using namespace std;

//-------------------------------------------AccNode Functions-----------------------------------//

AccNode::AccNode(char* nam, int val){
	int length;
	length=strlen(nam);
	name = new char[length];
	strncpy(name,nam,length);
	value=val;
	transfers=NULL;
	next=NULL;
	//cout<<"A new AccNode has just been created"<<endl;
}

AccNode::~AccNode(){
	delete[] name;
	//cout<<"An AccNode hash just been destroyed"<<endl;
}

void AccNode::set_name(char* nam){
	strcpy(name,nam);
}

char * AccNode::get_name(){
	return name;
}

void AccNode::set_value(int val){
	value = val; 
}

int AccNode::get_value(){
	return value ;
}

void AccNode::set_next(AccNode* node){
	next=node;
}

AccNode* AccNode::get_next(){
	return next;
}

void AccNode::insert_transfer(char * target, int val){
	if(transfers==NULL){
		transfers=new TrList();
		transfers->push_back(target,val);
		return ;
	}
	if(transfers->exists(target)==0){
		cout<<"Error:This tranfer already exists. Update?"<<endl;
		return ;
	}
	transfers->push_back(target,val);
}

void AccNode::print_transfers(){
	cout<<"My transfers are :"<<endl;
	transfers->print_list();
}

void AccNode::print_node(){
	cout<<"Name : "<<name<<endl;
	cout<<"Balance : "<<value<<endl;
	print_transfers();
}

TrList * AccNode::get_transfers(){
	return transfers;
}


//-------------------------------------------AccList Functions-----------------------------------//


AccList::AccList(){
	head=NULL;
	tail=NULL;
	size=0;
	//cout<<"A new AccList has just been created"<<endl;
}

AccList::~AccList(){
	AccNode * temp;
	while(head!=NULL){
		temp=pop();
		delete temp;
	}
	//cout<<"An AccList has just been destroyed"<<endl;
}

AccNode * AccList::pop(){
	AccNode * temp;
	if(size==0){
		return NULL;
	}
	else{
		temp=head;
		head=temp->get_next();
		size--;
		return temp;
	}
}

void AccList::push_back(char* key ,int val){
	AccNode * temp;
	temp = new AccNode(key,val);
	if(head==NULL){
		head=temp;
		tail=temp;	
	}
	else{
		tail->set_next(temp);
		tail=temp;
	}
	size++;
}

int AccList::exists(char* key){												//check if a name exists
	AccNode * temp;
	if(size==0)
		return 1;
	else{
		temp=head;
		while(temp!=NULL){
			if(strncmp(temp->get_name(),key,strlen(key))==0){
				return 0;
			}
			temp=temp->get_next();
		}
		return 1;
	}
}

AccNode* AccList::exists2(char* key){										//return the node with name = key 
	AccNode * temp;
	if(size==0)
		return NULL;
	else{
		temp=head;
		while(temp!=NULL){
			if(strncmp(temp->get_name(),key,strlen(key))==0){
				return temp;
			}
			temp=temp->get_next();
		}
		return NULL;
	}
}

void AccList::print_list(){
	AccNode * temp;
	temp=head;
	while(temp!=NULL){
		temp->print_node();
		temp=temp->get_next();
	}
}