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

struct GeneratorStruct
{
	int type;
	int value;
};