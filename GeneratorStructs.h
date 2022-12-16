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
	string assembly() {
		return "mov";
	}
};


class Expression : public Operation {
public:
	string name;

	string assembly() {
		return "mov";
	}

};

class PassedArguments : public Operation {
public:

	PassedArguments(Node* node) : Operation(node) {};

	string assembly() {
		string result = "push eax\n";
		result += "mov eax, " + node->getFirst()->getToken().get_name() + "\n";
		result += "push eax\n";

		return result;
	}

};

class LeftOperand : public Operation {
public:

	LeftOperand(Node* node) : Operation(node) {};

	string assembly() {
		Node *next = node->getFirst();

		string result = "push eax\n";

		if (next->getName() == "<ÂÛÇÎÂ>") {
			Node* idToken = node->getFirst();

			result += PassedArguments(node->getNext()[1]).assembly() + "\n";
			result += "call " + idToken->getFirst()->getToken().get_name();

			return result;
		}

		if (next->getName() == "<ÏÎÑËÅÄ. ÖÈÔÐ>") {
			result += "move eax, " + node->getFirst()->getToken().get_name() + "\n";

			return result;
		}

		if (next->getName() == "<ÈÄÅÍÒÈÔÈÊÀÒÎÐ>") {
			result += "move eax, " + node->getFirst()->getToken().get_name() + "\n";

			return result;
		}
	}

};

class RightOperand : public Operation {
public:

	RightOperand(Node* node) : Operation(node) {};

	string assembly() {
		Node* next = node->getFirst();

		string result = "push ebx\n";

		if (next->getName() == "<ÂÛÇÎÂ>") {


			Node* idToken = node->getFirst();

			result += "push eax\n";
			result += PassedArguments(node->getNext()[1]).assembly() + "\n";
			result += "call " + idToken->getFirst()->getToken().get_name() + "\n";
			result += "mov ebx, eax\n";
			result += "pop eax";

			return result;
		}

		if (next->getName() == "<ÏÎÑËÅÄ. ÖÈÔÐ>") {
			result += "move ebx, " + node->getFirst()->getToken().get_name() + "\n";

			return result;
		}

		if (next->getName() == "<ÈÄÅÍÒÈÔÈÊÀÒÎÐ>") {
			result += "move ebx, " + node->getFirst()->getToken().get_name() + "\n";

			return result;
		}
	}

};

class LogicExpression : public Operation {
public:

	LogicExpression(Node* node) : Operation(node) {};

	string assembly() {
		string result = LeftOperand(node->getFirst()).assembly();

		return result;
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

		return result;
	}

};