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

class Generator
{
	Tree tree;

	Generator(Tree *tree) {
		this->tree = *tree;
	}
};