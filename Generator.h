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
	vector <Operation*> operations;
	vector <Operation*> defines;
	vector <Operation*> procDefines;

	Generator(Tree *tree) {
		this->tree = *tree;
	}

	void generate() {
		Node* mainOperaion = tree.getRoot()->getFirst()->getFirst(); // ROOT -> <S> -> <��������>
		
		addOperation(mainOperaion);

		for (auto op : defines) {
			cout << op->assembly();
		}

		for (auto op : operations) {
			cout << op->assembly();
		}

		for (auto op : procDefines) {
			cout << op->assembly();
		}
	}

	void addOperation(Node* operation) {
		string name = operation->getName();

		if (name == "<��������>" || name == "<��������>") {
			for (auto el : operation->getNext()) addOperation(el);
		}

		else if (name == "<���������� ��������>") {
			operations.push_back(new LogicExpression(operation));
		} 

		else if (name == "<����>") {
			operations.push_back(new While(operation));
		}
		
		else if (operation->getName() == "<���������� ����������>") {
			defines.push_back(new Define(operation));
		}

		else if (operation->getName() == "<���������� �������>") {
			procDefines.push_back(new FunctionDefine(operation));
		}

		else {
			//throw InterpritationError("�� ������� �������� �������������");
		}
	}
};