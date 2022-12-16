#pragma once

#include <string>
#include <vector>

#include "AllStruct.h"

using namespace std;

enum { COMPLETED, LEFT_UNCOMPLETED, RIGHT_UNCOMPLETED, UNCOMPLETED};

class Node
{
public:

	Node(int id, string name, Token* token, Node* prev) {
		this->id = id;
		this->name = name;
		this->token = token;
		this->prev = prev;
	}

	~Node() {
		for (auto el : next) {
			delete el;
		}
	}

	string getName() {
		return name;
	}

	Token getToken() {
		return *token;
	}

	void addNext(Node* node) {
		next.push_back(node);
	}

	void setPrev(Node* node) {
		prev = node;
	}

	vector<Node*> getNext() {
		return next;
	}

	Node* getPrev() {
		return prev;
	}

	int getId() {
		return id;
	}

	int getLine() {
		return token->get_line_number();
	}

	Node* clone(Node* newPrev = nullptr) {

		Node* node = new Node(id, name, token, newPrev);

		for (auto el : next) {
			node->addNext(el->clone(node));
		}

		return node;
	}



private:
	int id;

	Token* token;
	string name;

	Node* prev = nullptr;
	vector<Node*> next;
};