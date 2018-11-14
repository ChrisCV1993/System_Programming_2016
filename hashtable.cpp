#include <iostream>
#include <cstdlib>
#include <cstring>
#include "hashtable.hpp"

using namespace std;

//---------------------------------------------Bucket Functions----------------------------------//

Bucket::Bucket(){
	accounts=NULL;
	//cout<<"A new bucket has just been created"<<endl;
}

Bucket::~Bucket(){
	if(accounts!=NULL)
		delete accounts;
	//cout<<"A bucket has just been destroyed"<<endl;
}

void Bucket::print_bucket(){
	cout<<"I contain :"<<endl;
	accounts->print_list();
}

int Bucket::insert_bucket(char * name , int value){
	if(accounts==NULL){
		accounts=new AccList();
		accounts->push_back(name,value);
		return 0;
	}
	if(accounts->exists(name)==0){
		cout<<"Error: This account already exists!"<<endl;
		return 1;
	}
	accounts->push_back(name,value);
	return 0;
}

AccList * Bucket::get_lista(){
	return accounts;
}


//------------------------------------------Hashtable Functions----------------------------------//

Hashtable::Hashtable(int numb){
	int i;
	size=numb;
	table = new Bucket*[size];
	for(i=0;i<size;i++){
		table[i] = NULL;
	} 
	cout<<"A new hashtable has just been created"<<endl;
}

Hashtable::~Hashtable(){
	int i;
	for(i=0;i<size;i++){
		if(table[i]!=NULL){
			delete table[i];
		}
		table[i]=NULL;
	}
	delete[] table;
	cout<<"A hashtable has just been destroyed"<<endl;

}

int Hashtable::hash_function(char * key){								//find the ascii of each letter and mod by size
	int length;
	int i;
	int sum=0;
	length = strlen(key);
	for(i=0;i<length;i++){
		sum = sum + key[i];
	}
	return sum%size;
}

int Hashtable::insert_hash(char * name,int value){
	int pos;
	pos = hash_function(name);
	//cout<<pos<<endl;
	if (table[pos]==NULL){
		table[pos]=new Bucket();
	}
	return table[pos]->insert_bucket(name,value);
}

int Hashtable::get_size(){
	return size;
}

void Hashtable::print_hash(){
	int i;
	for (i=0;i<size;i++){
		if (table[i]!=NULL)
			table[i]->print_bucket();
	}
}

AccNode * Hashtable::lookupnode(char * name){							//search for a specific node in the bucket
	AccNode * temp;
	AccList * lista;
	int pos;
	pos = hash_function(name);

	if(table[pos]==NULL)
		return NULL;
	lista = table[pos]->get_lista();
	if(lista==NULL)
		return NULL;
	return lista->exists2(name);
}
