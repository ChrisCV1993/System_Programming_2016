using namespace std;

#ifndef LISTA_HPP
	#define LISTA_HPP


class TrNode{
	char * target;
	int value;
	TrNode * next;

	public:
		TrNode(char* , int);
		~TrNode();
		void set_target(char*);
		char * get_target();
		void set_value(int);
		int get_value();
		void set_next(TrNode*);
		TrNode* get_next();
		void print_node();
};


class TrList{
	TrNode * head;
	TrNode * tail;
	int size;

	public:
		TrList();
		~TrList();
		TrNode * pop();
		void push_back(char*,int);
		int exists(char*);								//just check
		void print_list();
		TrNode * exists2(char*);						//if exists return it 
};

#endif