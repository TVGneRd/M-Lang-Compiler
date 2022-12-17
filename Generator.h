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

	Generator(Tree *tree) {
		this->tree = *tree;
	}

	void defineSubProcess() {

	}

	void generate() {
		Node* mainOperaion = tree.getRoot()->getFirst()->getFirst(); // ROOT -> <S> -> <��������>
		
		addOperation(mainOperaion);

		for (auto op : operations) {
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
			operations.push_back(new Define(operation));
		}

		else {
			//throw InterpritationError("�� ������� �������� �������������");
		}
	}
};