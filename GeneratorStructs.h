#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <format>
#include <iomanip>

#include "AllStruct.h"

enum {MARK, BOOL, OPERATIONS, CONST, ID_VALUE, ID_REFENCE};

class Operation {
public:
	Operation* next;

	Node* node;

	Operation(Node* node) {
		this->node = node;
	}

	virtual void convolution() = 0;
	virtual string assembly() = 0;

private:

};

class Assigment: public Operation {
public:
	void convolution() {

	}

	string assembly() {
		return "mov";
	}
};


class Expression : public Operation {
public:
	string name;

	void convolution() {
		//node->getName()
	}

	string assembly() {
		return "mov";
	}

};


class LogicExpression : public Operation {
public:

	LogicExpression(Node* node) : Operation(node) {
		convolution();
	};

	void convolution() {
		//node->getName()
	}

	string assembly() {
		return "";
	}

};

class While : public Operation {
public:

	While(Node* node) : Operation(node) {
		convolution();
	};

	void convolution() {
		
	}

	string assembly() {
		string id1 = "while_begin_" + node->getId();
		string id2 = "while_end_" + node->getId();

		string result = id1 + ":\n";
		result += LogicExpression(node->getFirst()).assembly() + " " + id2;
		result += id2 + ":\n";
	}

};