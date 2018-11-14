#include "lista.hpp"

using namespace std;

#ifndef HASHTABLE_HPP
	#define HASHTABLE_HPP


class Bucket{
	AccList * accounts;

	public:
		Bucket();
		~Bucket();
		void print_bucket();
		int insert_bucket(char *,int);
		AccList * get_lista();
};

class Hashtable{
	Bucket ** table;
	int size;

	public:
		Hashtable(int);
		~Hashtable();
		int hash_function(char*);
		int insert_hash(char *,int);					//insert to hash
		int get_size();
		void print_hash();
		AccNode * lookupnode(char *);					//find spec node
};

#endif