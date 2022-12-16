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

	virtual string assembly() = 0;

private:

};

class Assigment: public Operation {
public:

	string assembly() { // Присвоение (левому операнду)
		string result = "";
		result += "mov ecx, " + RightOperand(node->getFirst()).assembly() + "\n" + "mov " + LeftOperand(node->getFirst()).assembly() + ", ecx" + "\n";
		return "mov";
	}
	/*mov ecx, a
	mov b, ecx*/
};


class Expression : public Operation {
public:
	string name;

	string assembly() {

		return "mov";
	}
	
};

//class Define : public Operation {
//public:
//	Define(Node* node) : Operation(node) {};
//	string result = "";
//	string assembly() {
//		if ()
//		return "mov";
//	}
//
//};

class LogicExpression : public Operation {
public:

	LogicExpression(Node* node) : Operation(node) {};

	string assembly() {
		return "";
	}

};

class While : public Operation {
public:

	While(Node* node) : Operation(node) {};

	string assembly() {
		string id1 = "while_begin_" + node->getId();
		string id2 = "while_end_" + node->getId();

		string result = id1 + ":\n";
		result += LogicExpression(node->getFirst()).assembly() + " " + id2;
		result += id2 + ":\n";
	}

};