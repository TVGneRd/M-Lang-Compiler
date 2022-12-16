#pragma once

#include <string>
#include <vector>

using namespace std;

enum { COMPLETED, LEFT_UNCOMPLETED, RIGHT_UNCOMPLETED, UNCOMPLETED};

class Node
{
public:

	Node(int id, string name, int line, Node* prev) {
		this->id = id;
		this->line = line;
		this->name = name;
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
		return line;
	}

	Node* clone(Node* newPrev = nullptr) {

		Node* node = new Node(id, name, line, newPrev);

		for (auto el : next) {
			node->addNext(el->clone(node));
		}

		return node;
	}



private:
	int id;
	int line;

	string name;

	Node* prev = nullptr;
	vector<Node*> next;
};