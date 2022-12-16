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
	vector <GeneratorStruct> lexems;

	Generator(Tree *tree) {
		this->tree = *tree;
	}

	void defineSubProcess() {

	}

	void genarate() {
		defineSubProcess();

		Node* mainOperaions = tree.getRoot()->getNext()[0]->getNext()[0]->getNext()[0]; // ROOT -> <S> -> <ÎÏÅÐÀÖÈÈ>
	}
};