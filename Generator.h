#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <format>
#include <iomanip>

#include "AllStruct.h"
#include "Tree.h"
#include "Node.h"
#include "GeneratorStructs.h"

class Generator
{

public:
	Tree tree;
	vector <OperationBase*> operations;
	vector <OperationBase*> defines;
	vector <OperationBase*> procDefines;

	Generator(Tree *tree) {
		this->tree = *tree;
	}

	void generate() {
		Node* mainOperaion = tree.getRoot()->getFirst()->getFirst(); // ROOT -> <S> -> <ÎÏÅĞÀÖÈÈ>
		
		addOperation(mainOperaion);
		cout << ".model small" << endl;
		cout << ".stack 100h" << endl;
		cout << ".data" << endl;

		for (auto op : defines) {
			cout << op->assembly();
		}

		cout << endl << ".code" << endl;
		cout << "main proc" << endl;

		for (auto op : operations) {
			cout << op->assembly() << endl;
		}
		cout << "end main" << endl;

		for (auto op : procDefines) {
			cout << op->assembly() << endl;
			cout << getSystemFunctionsDefine() << endl;
		}
	}

	void addOperation(Node* operation) {
		string name = operation->getName();

		if (name == "<ÎÏÅĞÀÖÈÈ>" || name == "<ÎÏÅĞÀÖÈß>") {
			for (auto el : operation->getNext()) addOperation(el);
		}

		else if (name == "<ËÎÃÈ×ÅÑÊÀß ÎÏÅĞÀÖÈß>") {
			operations.push_back(new LogicExpression(operation));
		} 

		else if (name == "<ÖÈÊË>") {
			operations.push_back(new While(operation));
		}

		else if (name == "<ÏĞÈÑÂÎÅÍÈÅ>") {
			operations.push_back(new Assigment(operation));
		}

		else if (name == "<ÂÛÇÎÂ>") {
			operations.push_back(new Call(operation));
		}

		else if (operation->getName() == "<ÎÁÚßÂËÅÍÈÅ ÏÅĞÅÌÅÍÍÎÉ>") {
			defines.push_back(new Define(operation));
		}

		else if (operation->getName() == "<ÎÁÚßÂËÅÍÈÅ ÔÓÍÊÖÈÈ>") {
			procDefines.push_back(new FunctionDefine(operation));
		}

		else {
			throw InterpritationError("Íå óäàëîñü ïğîâåñòè èíòğåïğåòàöèş");
		}
	}


	string getSystemFunctionsDefine() {

		string result = "print PROC\n";
		result += "aam \n add ax, 3030h \n mov dl, ah \n mov dh, al \n mov ah, 02 \n int 21h \n mov dl, dh \n int 21h\n";
		result += "pop ebx\n";
		result += "RET\n";
		result += "print ENDP\n";

		return result;
	}
};