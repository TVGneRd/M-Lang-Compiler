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
		Node* mainOperaion = tree.getRoot()->getFirst()->getFirst(); // ROOT -> <S> -> <ÎÏÅĞÀÖÈÈ>
		
		addOperation(mainOperaion);

		for (auto op : operations) {
			cout << op->assembly();
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
		
		else if (operation->getName() == "<ÎÁÚßÂËÅÍÈÅ ÏÅĞÅÌÅÍÍÎÉ>") {
			operations.push_back(new Define(operation));
		}

		else {
			//throw InterpritationError("Íå óäàëîñü ïğîâåñòè èíòğåïğåòàöèş");
		}
	}
};