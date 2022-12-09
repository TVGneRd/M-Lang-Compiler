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
#include "ParseErrors.h"

using namespace std;

template<class C, typename T>
bool contains(C&& c, T e) { return find(begin(c), end(c), e) != end(c); };

class Parser
{
	Table<Token> stream;
	vector<Variable> variables;
	vector<vector<Variable>> variablesStore;

	vector<int> streamStore;

	Tree tree;
	vector<Tree> treeStore;

	string TYPES[3] = { "char", "int", "string" };
	string MATH_OPERATORS[4] = { "+", "-", "*", "/"};
	string LOGIC_OPERATORS[6] = { "<", ">", "<=", ">=", "!=", "=="};

	//
	// Вспомогательные методы 
	//

	// Вывод ошибки
	void ErrorOnToken(Token t, string message) {
		throw SyntaxError(message, t.get_line_number());
	}

	// Запоминает состояние потока/дерева
	void store() {
 		treeStore.push_back(tree);
		streamStore.push_back(stream.getIterrator());
		variablesStore.push_back(variables);
	}

	// Удаляет состоние потока/дерева
	void dropStore() {
		streamStore.pop_back();
		treeStore.pop_back();
		variablesStore.pop_back();
	}

	// Востанавливает состояние потока/дерева
	void restore() {
		stream.seek(streamStore.back());
		tree.free();
		tree = treeStore.back();
		variables = variablesStore.back();

		dropStore();
	}

	// Добавление переменной в таблицу
	void addVariable(string type, string name) {
		Variable var = Variable(name, type);

		if (checkVariableExists(name)) throw SematnicError(format("Идентификатор \033[30;35m{}\033[30;0m определен ранее", name), stream.getCurrentLine());

		variables.push_back(var);
	}

	// Проверка существования перменной в таблице
	bool checkVariableExists(string name) {
		for (auto var : variables) {
			if (var.get_name() == name) return true;
		}

		return false;
	}

	//
	// Вспомогательные методы для работы с лексемами
	//

	// Следующая лексема
	void nextLexem(string name)
	{
		Token token = stream.get();
		if (token != name) ErrorOnToken(token, format("Ожидалось {}", name));
	}

	// Следующий тип лексемы
	void nextLexemType(int lexemType)
	{
		Token token = stream.get();
		string lexemTypesName[] = {"Идентификатор", "Ключевое слово", "Разделитель", "Числовая константа"};
		if (token.get_type() != lexemType) ErrorOnToken(token, format("Ожидалось {}", lexemTypesName[lexemType]));
	}

	// Следующий тип
	void nextType()
	{
		Token token = stream.get();
		if (!contains(TYPES, token.get_name())) ErrorOnToken(token, "Ожидался тип");
	}

	// Следующий идентификатор
	void nextIdentifier()
	{
		Token token = stream.get();
		if (token.get_type() != IDENTIFIER) ErrorOnToken(token, "Ожидался идентификатор");
	}

	// Следующий мат. оператор
	void nextMathOperator()
	{
		Token token = stream.get();
		if (!contains(MATH_OPERATORS, token.get_name())) ErrorOnToken(token, "Ожидался мат. оператор");
	}

	// Следующий лог. оператор
	void nextLogicOperator()
	{
		Token token = stream.get();
		if (!contains(LOGIC_OPERATORS, token.get_name())) ErrorOnToken(token, "Ожидался логический оператор");
	}

	//
	// Обработка правил грамматики
	//

	// <S> = “main” ”(” “)” “;” “{”<ОПЕРАЦИИ>”}”
	void S(void)
	{
		tree.down("<S>", stream.getCurrentLine());

		nextLexem("main");
		nextLexem("(");
		nextLexem(")");
		nextLexem(";");
		nextLexem("{");
		Operations();
		nextLexem("}");

		tree.up();
	}

	// <ОПЕРАЦИИ> = <ОПЕРАЦИЯ>”;”<ОПЕРАЦИИ>”;” | <ОПЕРАЦИЯ>”;”
	void Operations(){
		tree.down("<ОПЕРАЦИИ>", stream.getCurrentLine());

		store();

		try{
			Operation();
			nextLexem(";");
			Operations();

			tree.up();
			return dropStore();

		} catch (SyntaxError err) { restore(); };

		Operation();
		nextLexem(";");

		tree.up();
	}

	// <ОПЕРАЦИЯ> = <ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ> | <ОБЪЯВЛЕНИЕ ФУНКЦИИ> | <ПРИСВОЕНИЕ> | <ВЫЗОВ> | <ЦИКЛ>
	void Operation() {
		tree.down("<ОПЕРАЦИЯ>", stream.getCurrentLine());
		
		store();

		// <ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ>
		try {
			VariableDefine();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <ПРИСВОЕНИЕ>
		try {
			Assignment();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <ЦИКЛ>
		try {
			While();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();
		// <ВЫЗОВ>
		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		// <ОБЪЯВЛЕНИЕ ФУНКЦИИ>
		FunctionDefine();
		
		tree.up();
	}

	// <ЦИКЛ> = “while””(”<ЛОГИЧЕСКАЯ ОПЕРАЦИЯ>”)”<ОПЕРAЦИЯ>”;”
	void While() {
		tree.down("<ЦИКЛ>", stream.getCurrentLine());

		nextLexem("while");
		nextLexem("(");
		LogicExpression();
		nextLexem(")");
		Operation();

		tree.up();
	}

	// <ЛОГИЧЕСКАЯ ОПЕРАЦИЯ> = <ОПЕРАНД><ЛОГИЧЕСКИЙ ОПЕРАТОР><ОПЕРАНД>
	void LogicExpression() {
		tree.down("<ЛОГИЧЕСКАЯ ОПЕРАЦИЯ>", stream.getCurrentLine());

		Operand();
		nextLogicOperator();
		Operand();

		tree.up();
	}

	// <ОБЪЯВЛЕНИЕ ПЕРЕМЕННЫХ> = <ТИП><ИДЕНТИФИКАТОР>
	void VariableDefine() {
		tree.down("<ОБЪЯВЛЕНИЕ ПЕРЕМЕННОЙ>", stream.getCurrentLine());

		Token type = stream.get();
		Token identifyer = stream.get();
		stream.back(2);

		nextType();
		nextIdentifier();

		addVariable(type.get_name(), identifyer.get_name());

		tree.up();
	}

	// <ОБЪЯВЛЕНИЕ ФУНКЦИИ> = “proc”<ИДЕНТИФИКАТОР>”(”< ПОСЛЕД. ПРИН. АРГУМЕНТОВ> ”)” ”{“ <ОПЕРАЦИИ><ВОЗВРАТ>”}” | “proc”<ИДЕНТИФИКАТОР>”(”< ПОСЛЕД. ПРИН. АРГУМЕНТОВ>”)””{“<ВОЗВРАТ>”}”
	void FunctionDefine() {
		tree.down("<ОБЪЯВЛЕНИЕ ФУНКЦИИ>", stream.getCurrentLine());

		nextLexem("proc");
		nextIdentifier();

		nextLexem("(");
		AcceptedArguments();
		nextLexem(")");

		nextLexem("{");

		store();
		// <ОПЕРАЦИИ>
		try {
			Operations();
			dropStore();
		}
		catch (SyntaxError err) { restore(); };
		
		Return();
		nextLexem(";");

		nextLexem("}");

		tree.up();
	}

	// <ПОСЛЕД. ПРИН. АРГУМЕНТОВ> = <ПРИН. АРГУМЕНТ>|< ПРИН. АРГУМЕНТ>”,”<ПОСЛЕД. ПРИН. АРГУМЕНТОВ>
	void AcceptedArguments() {
		tree.down("<ПОСЛЕД. ПРИН. АРГУМЕНТОВ>", stream.getCurrentLine());

		store();

		// <ИДЕНТИФИКАТОР>”,”<ПОСЛЕД. ПЕРЕД. АРГУМЕНТОВ>
		try {
			nextType();
			nextIdentifier();
			nextLexem(",");
			AcceptedArguments();

			tree.up();

			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		nextType();
		nextIdentifier();

		tree.up();
	}

	// <ВОЗВРАТ> = “return” ” “ <ОПЕРАНД>
	void Return() {
		tree.down("<ВОЗВРАТ>", stream.getCurrentLine());
		
		nextLexem("return");
		Operand();

		tree.up();
	}

	// <ПРИСВОЕНИЕ> = <ИДЕНТИФИКАТОР>”=”<ВЫЧИСЛЯЕМОЕ ЗНАЧЕНИЕ>
	void Assignment() {
		tree.down("<ПРИСВОЕНИЕ>", stream.getCurrentLine());

		nextIdentifier();
		nextLexem("=");
		Expression();

		tree.up();
	}

	// <ВЫЧИСЛЯЕМОЕ ЗНАЧЕНИЕ> = <ВЫЗОВ> | <ОПЕРАНД><МАТ. ОПЕРАТОР><ВЫЧИСЛЯЕМОЕ ЗНАЧЕНИЕ>|<ОПЕРАНД>
	void Expression() {
		tree.down("<ВЫЧИСЛЯЕМОЕ ЗНАЧЕНИЕ>", stream.getCurrentLine());

		store();

		// <ВЫЗОВ>
		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <ОПЕРАНД><МАТ. ОПЕРАТОР><ВЫЧИСЛЯЕМОЕ ЗНАЧЕНИЕ>
		try {
			Operand();
			nextMathOperator();
			Expression();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };
		
		Operand();

		tree.up();
	}

	// <ОПЕРАНД> = <ПОСЛЕД. ЦИФР> | <ИДЕНТИФИКАТОР> | <ВЫЗОВ>
	void Operand() {
		tree.down("<ОПЕРАНД>", stream.getCurrentLine());

		Token token = stream.get();
		
		if (token.get_type() == CONSTANT) {
			tree.up();
			return;
		}

		stream.back();

		store();

		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		nextIdentifier();

		tree.up();
	}


	// <ВЫЗОВ> = <ИДЕНТИФИКАТОР>”(”<ПОСЛЕД. ПЕРЕД. АРГУМЕНТОВ>”)”
	void Call() {
		tree.down("<ВЫЗОВ>", stream.getCurrentLine());
		nextIdentifier();
		nextLexem("(");
		PassedArguments();
		nextLexem(")");
		tree.up();
	}

	// <ПОСЛЕД. ПЕРЕД. АРГУМЕНТОВ> = <ИДЕНТИФИКАТОР>|<ИДЕНТИФИКАТОР>”,”<ПОСЛЕД. ПЕРЕД. АРГУМЕНТОВ>
	void PassedArguments() {
		tree.down("<ПОСЛЕД. ПЕРЕД. АРГУМЕНТОВ>", stream.getCurrentLine());

		store();

		// <ИДЕНТИФИКАТОР>”,”<ПОСЛЕД. ПЕРЕД. АРГУМЕНТОВ>
		try {
			nextIdentifier();
			nextLexem(",");
			PassedArguments();

			tree.up();

			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		nextIdentifier();
		
		tree.up();
	}

public:
	Parser(Table<Token>* stream_of_token) {
		stream = *stream_of_token;
	}

	void printTree() {
		tree.print();
	}

	void printVariablesTable() {
		cout << setw(10) << "Тип" << setw(20) << "Идентификатор" << endl;

		for (auto var : variables)
			cout <<  setw(10) << var.get_type() << setw(20) << var.get_name() << endl;
	}

	void parse() {
		stream.seek(0);
		S();
	}

};

