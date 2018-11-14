#include "trlista.hpp"

using namespace std;

#ifndef TRLISTA_HPP
	#define TRLISTA_HPP


class AccNode{
	char * name;
	int value;
	TrList * transfers;
	AccNode * next;

	public:
		AccNode(char* , int);
		~AccNode();
		void set_name(char*);
		char * get_name();
		void set_value(int);
		int get_value();
		void set_next(AccNode*);
		AccNode* get_next();
		void insert_transfer(char*,int);
		void print_transfers();
		void print_node();
		TrList * get_transfers();
};


class AccList{
	AccNode * head;
	AccNode * tail;
	int size;

	public:
		AccList();
		~AccList();
		AccNode * pop();
		void push_back(char*,int);
		int exists(char*);							//check if exists
		void print_list();
		AccNode* exists2(char* );					//return the node with the name 

};

#endif
