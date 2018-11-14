#include <iostream>
#include <cstdlib>
#include <cstring>
#include "trlista.hpp"

using namespace std;

//--------------------------------------------TrNode Functions-----------------------------------//


TrNode::TrNode(char* nam , int val){
	int length;
	length=strlen(nam);
	target = new char[length];
	strncpy(target,nam,length);
	value=val;
	next=NULL;
	//cout<<"A new TrNode has just been created"<<endl;
}

TrNode::~TrNode(){
	delete[] target;
	//cout<<"An TrNode hash just been destroyed"<<endl;
}

void TrNode::set_target(char* name){
	strcpy(target,name);
}

char * TrNode::get_target(){
	return target;
}

void TrNode::set_value(int val){
	value=val;
}

int TrNode::get_value(){
	return value;
}

void TrNode::set_next(TrNode* temp){
	next=temp;
}

TrNode* TrNode::get_next(){
	return next;
}

void TrNode::print_node(){
	cout<<"To : "<<target<<endl;
	cout<<"Value : "<<value<<endl;
	cout<<"-----------------------"<<endl;
}

//--------------------------------------------TrList Functions-----------------------------------//

TrList::TrList(){
	head=NULL;
	tail=NULL;
	size=0;
	//cout<<"A new TrList hash just been created"<<endl;
}

TrList::~TrList(){
	TrNode * temp;
	while(head!=NULL){
		temp=pop();
		delete temp;
	}
	//cout<<"A TrList has just been destroyed"<<endl;
}

TrNode * TrList::pop(){
	TrNode * temp;
	if(size==0)
		return NULL;
	temp=head;
	head=temp->get_next();
	size--;
	return temp;
}

void TrList::push_back(char* tgt,int val){
	TrNode * temp;
	temp=new TrNode(tgt,val);
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

int TrList::exists(char* tgt){										//if exists
	TrNode * temp;
	if(size==0)
		return 1;
	else{
		temp=head;
		while(temp!=NULL){
			if(strncmp(temp->get_target(),tgt,strlen(tgt))==0){
				return 0;
			}
			temp=temp->get_next();
		}
		return 1;
	}

}

TrNode * TrList::exists2(char* tgt){								//return the node 
	TrNode * temp;
	if(size==0)
		return NULL;
	else{
		temp=head;
		while(temp!=NULL){
			if(strncmp(temp->get_target(),tgt,strlen(tgt))==0){
				return temp;
			}
			temp=temp->get_next();
		}
		return NULL;
	}
}

void TrList::print_list(){
	TrNode * temp;
	temp=head;
	while(temp!=NULL){
		temp->print_node();
		temp=temp->get_next();
	}
}