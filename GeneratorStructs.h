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

class Expression : public Operation {
public:
	Expression(Node* node) : Operation(node) {};

	string assembly() {
		string result;
		Node* next = node->getFirst();

		if (next->getName() == "<ÂÛÇÎÂ>") {
			/*Node* idToken = next->getFirst();

			result += PassedArguments(next->getNext()[1]).assembly() + "\n";
			result += "call " + idToken->getToken().get_name();*/

			return result;
		}
	}

};


class Define : public Operation {
public:
	Define(Node* node) : Operation(node) {};
	string assembly() {
		/*Node* next = node->getFirst();*/
		string result = "";
		if (node->getNext()[0]->getToken().get_name() == "char" || node->getNext()[0]->getToken().get_name() == "string"){
			result += node->getNext()[1]->getToken().get_name() + " DB \n";
		}
		else 
			result += node->getNext()[1]->getToken().get_name() + " DD \n"; 
		return result;
	}
};


class LeftOperand : public Operation {
public:

	LeftOperand(Node* node) : Operation(node) {};

	string assembly() {
		Node *next = node->getFirst();

		string result ;

		if (next->getName() == "<ÂÛÇÎÂ>") {
			Node* idToken = next->getFirst();

			result += PassedArguments(next->getNext()[1]).assembly() + "\n";
			result += "call " + idToken->getToken().get_name();

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

		string result;

		if (next->getName() == "<ÂÛÇÎÂ>") {

			Node* idToken = next->getFirst();

			result += "push eax\n";
			result += PassedArguments(node->getNext()[1]).assembly() + "\n";
			result += "call " + idToken->getFirst()->getToken().get_name() + "\n";
			result += "mov ebx, eax\n";
			result += "pop eax";

			return result;
		}

		if (next->getToken().get_type() == CONSTANT) {
			result += "move ebx, " + node->getFirst()->getToken().get_name() + "\n";

			return result;
		}

		if (next->getName() == "<ÈÄÅÍÒÈÔÈÊÀÒÎÐ>") {
			result += "move ebx, " + node->getFirst()->getToken().get_name() + "\n";

			return result;
		}
	}

};

class FunctionDefine : public Operation {
public:
	FunctionDefine(Node* node) : Operation(node) {};
	string assembly() {
		Node* idToken = node->getFirst();

		string result = idToken->getToken().get_name() + " PROC\n";
		result += RightOperand(node->getNext()[2]).assembly();
		result += "mov eax, ebx\n";
		result += "pop ebx\n";
		result += "RET\n";
		result += idToken->getToken().get_name() + " PROC\n";

		return result;
	}

};

class Assigment : public Operation {
public:
	Assigment(Node* node) : Operation(node) {};

	string assembly() { // Ïðèñâîåíèå (ëåâîìó îïåðàíäó)
		string result;
		Node* id = node->getFirst();
		result += Expression(node->getNext()[1]).assembly() + "\n";

		return result;
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