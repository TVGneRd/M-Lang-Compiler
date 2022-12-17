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

class OperationBase {
public:
	OperationBase* next;

	Node* node;

	OperationBase(Node* node) {
		this->node = node;
	}

	virtual string assembly() = 0;

private:

};

class PassedArguments : public OperationBase {
public:

	PassedArguments(Node* node) : OperationBase(node) {};

	string assembly() {
		string result = "push eax\n";
		result += "mov eax, " + node->getFirst()->getToken().get_name() + "\n";
		result += "push eax\n";

		return result;
	}

};



class Expression : public OperationBase {
public:
	Expression(Node* node) : OperationBase(node) {};

	string assembly() {
		string result;
		Node* next = node->getFirst();

		if (next->getName() == "<ÂÛÇÎÂ>") {
			Node* idToken = next->getFirst();

			result += PassedArguments(next->getNext()[1]).assembly() + "\n";
			result += "call " + idToken->getToken().get_name();

			return result;
		} 
		else if (next->getName() == "<ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>") {
			Node* idToken = next->getFirst();

			if (node->getNext().size() == 1) {
				result += idToken->getToken().get_name();
			}
			else {
				if (next->getNext()[1]->getToken().get_name() == "+") {
					result += "add eax," + idToken->getToken().get_name() + "\n";
				}
				else if (next->getNext()[1]->getToken().get_name() == "-") {
					result += "sub eax," + idToken->getToken().get_name() + "\n";
				}
				else if (next->getNext()[1]->getToken().get_name() == "*") {
				result += "mov eax," + idToken->getToken().get_name() + "\n" + "mul eax";
				}
				result += Expression(node->getNext()[1]).assembly();
			}
			return result;
		}
		else if (next->getName() == "<ÎÏÅÐÀÍÄ>") {
			Node* idToken = next->getFirst();
			result += idToken->getToken().get_name();

			if (node->getNext().size() == 0) {
				return result;
			}
			else result += Expression(node->getNext()[1]).assembly();
		}
	}
};


class Define : public OperationBase {
public:
	Define(Node* node) : OperationBase(node) {};
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


class LeftOperand : public OperationBase {
public:

	LeftOperand(Node* node) : OperationBase(node) {};

	string assembly() {
		Node *next = node->getFirst();

		string result ;

		if (next->getName() == "<ÂÛÇÎÂ>") {
			Node* idToken = next->getFirst();

			result += PassedArguments(next->getNext()[1]).assembly() + "\n";
			result += format("call {}\n", idToken->getToken().get_name()); 

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


class RightOperand : public OperationBase {
public:

	RightOperand(Node* node) : OperationBase(node) {};

	string assembly() {
		Node* next = node->getFirst();

		string result;

		if (next->getName() == "<ÂÛÇÎÂ>") {

			Node* idToken = next->getFirst();

			result += "push eax\n";
			result += PassedArguments(node->getNext()[1]).assembly() + string("\n");
			result += format("call {}\n", idToken->getFirst()->getToken().get_name());
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

class FunctionDefine : public OperationBase {
public:
	FunctionDefine(Node* node) : OperationBase(node) {};
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

class Assigment : public OperationBase {
public:
	Assigment(Node* node) : OperationBase(node) {};

	string assembly() { // Ïðèñâîåíèå (ëåâîìó îïåðàíäó)
		string result;
		Node* id = node->getFirst();
		result += "mov eax," + Expression(node->getNext()[1]).assembly() + "\n";
		result +=  node->getFirst()->getToken().get_name() + ", eax\n";
		return result;
	}
	//mov eax, result
	//mov operand_left, eax
};


class LogicExpression : public OperationBase {
public:

	LogicExpression(Node* node) : OperationBase(node) {};

	string assembly() {
		string result = LeftOperand(node->getFirst()).assembly();

		return result;
	}
};

class Operation : public OperationBase {
public:
	Operation(Node* node) : OperationBase(node) {};

	string assembly() {
		string name = node->getName();

		if (name == "<ÎÏÅÐÀÖÈÈ>" || name == "<ÎÏÅÐÀÖÈß>") {
			string result = "";
			for (auto el : node->getNext()) result += Operation(el).assembly();
			return result;
		}

		else if (name == "<ÏÐÈÑÂÎÅÍÈÅ>") {
			return Assigment(node).assembly();
		}

		else {
			throw InterpritationError("Íå óäàëîñü ïðîâåñòè èíòðåïðåòàöèþ");
		}
	};

private:

};

class While : public OperationBase {
public:

	While(Node* node) : OperationBase(node) {};

	string assembly() {
		string id1 = "while_begin_" + to_string(node->getId());
		string id2 = "while_end_" + to_string(node->getId());

		string result = id1 + ":\n";
		result += LogicExpression(node->getFirst()).assembly() + " " + id2 + "\n";

		result += Operation(node->getNext()[1]).assembly();
		result += id2 + ":\n";

		return result;
	}

};