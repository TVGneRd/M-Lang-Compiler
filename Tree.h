#pragma once

#include <string>
#include <vector>
#include <iostream>

#include "Node.h"

using namespace std;

class Tree
{
public:
	Tree(Node* rootPtr = nullptr) {
		root = rootPtr ? rootPtr : new Node(counter++, "root", 0, rootPtr);
		ptr = root;
	}

	Tree(const Tree& tree) {
		root = tree.root->clone();
		ptr = findById(root, tree.ptr->getId());
		counter = tree.counter;
	}

	void free() {
		delete root;
	}

	Node* findById(Node* node, int id) {
		if (node->getId() == id) return node;

		for (auto nextEl : node->getNext()) {
			Node* value = findById(nextEl, id);
			if (value) return value;
		}

		return nullptr;
	}

	void down(string name, int line) {
		Node* node = new Node(counter++, name, line, ptr);
		ptr->addNext(node);
		ptr = node;
	}

	void up() {
		ptr = ptr->getPrev();
	}

	Node* getRoot() {
		return root;
	};

	void print() {
		printNode(root, 0);
	}

	void printNode(Node* node, int deep) {
		for (int i = 0; i < deep; i++) cout << "-";

		cout << "> " << node->getName() << " [" << node->getLine() << "]" << " (" << node->getNext().size() << ")" << endl;

		for (auto nextEl : node->getNext()) {
			printNode(nextEl, deep + 1);
		}
	}

private:
	Node* root;
	Node* ptr;
	int counter = 0;
};
