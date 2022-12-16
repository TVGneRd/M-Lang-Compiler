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
	// ��������������� ������ 
	//

	// ����� ������
	void ErrorOnToken(Token t, string message) {
		throw SyntaxError(message, t.get_line_number());
	}

	// ���������� ��������� ������/������
	void store() {
 		treeStore.push_back(tree);
		streamStore.push_back(stream.getIterrator());
		variablesStore.push_back(variables);
	}

	// ������� �������� ������/������
	void dropStore() {
		streamStore.pop_back();
		treeStore.pop_back();
		variablesStore.pop_back();
	}

	// �������������� ��������� ������/������
	void restore() {
		stream.seek(streamStore.back());
		tree.free();
		tree = treeStore.back();
		variables = variablesStore.back();

		dropStore();
	}

	// ���������� ���������� � �������
	void addVariable(string type, string name) {
		Variable var = Variable(name, type);

		if (checkVariableExists(name)) throw SematnicError(format("������������� \033[30;35m{}\033[30;0m ��������� �����", name), stream.getCurrentLine());

		variables.push_back(var);
	}

	// �������� ������������� ��������� � �������
	bool checkVariableExists(string name) {
		for (auto var : variables) {
			if (var.get_name() == name) return true;
		}

		return false;
	}

	//
	// ��������������� ������ ��� ������ � ���������
	//

	// ��������� �������
	void nextLexem(string name)
	{
		Token token = stream.get();
		if (token != name) ErrorOnToken(token, format("��������� {}", name));
	}

	// ��������� ��� �������
	void nextLexemType(int lexemType)
	{
		Token token = stream.get();
		string lexemTypesName[] = {"�������������", "�������� �����", "�����������", "�������� ���������"};
		if (token.get_type() != lexemType) ErrorOnToken(token, format("��������� {}", lexemTypesName[lexemType]));
	}

	// ��������� ���
	void nextType()
	{
		Token token = stream.get();
		if (!contains(TYPES, token.get_name())) ErrorOnToken(token, "�������� ���");
	}

	// ��������� �������������
	void nextIdentifier()
	{
		Token token = stream.get();
		if (token.get_type() != IDENTIFIER) ErrorOnToken(token, "�������� �������������");
	}

	// ��������� ���. ��������
	void nextMathOperator()
	{
		Token token = stream.get();
		if (!contains(MATH_OPERATORS, token.get_name())) ErrorOnToken(token, "�������� ���. ��������");
	}

	// ��������� ���. ��������
	void nextLogicOperator()
	{
		Token token = stream.get();
		if (!contains(LOGIC_OPERATORS, token.get_name())) ErrorOnToken(token, "�������� ���������� ��������");
	}

	//
	// ��������� ������ ����������
	//

	// <S> = �main� �(� �)� �;� �{�<��������>�}�
	void S()
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

	// <��������> = <��������>�;�<��������>�;� | <��������>�;�
	void Operations(){
		tree.down("<��������>", stream.getCurrentLine());

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

	// <��������> = <���������� ����������> | <���������� �������> | <����������> | <�����> | <����>
	void Operation() {
		tree.down("<��������>", stream.getCurrentLine());
		
		store();

		// <���������� ����������>
		try {
			VariableDefine();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <����������>
		try {
			Assignment();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <����>
		try {
			While();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();
		// <�����>
		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		// <���������� �������>
		FunctionDefine();
		
		tree.up();
	}

	// <����> = �while��(�<���������� ��������>�)�<����A���>�;�
	void While() {
		tree.down("<����>", stream.getCurrentLine());

		nextLexem("while");
		nextLexem("(");
		LogicExpression();
		nextLexem(")");
		Operation();

		tree.up();
	}

	// <���������� ��������> = <�������><���������� ��������><�������>
	void LogicExpression() {
		tree.down("<���������� ��������>", stream.getCurrentLine());

		Operand();
		nextLogicOperator();
		Operand();

		tree.up();
	}

	// <���������� ����������> = <���><�������������>
	void VariableDefine() {
		tree.down("<���������� ����������>", stream.getCurrentLine());

		Token type = stream.get();
		Token identifyer = stream.get();
		stream.back(2);

		nextType();
		nextIdentifier();

		addVariable(type.get_name(), identifyer.get_name());

		tree.up();
	}

	// <���������� �������> = �proc�<�������������>�(�< ������. ����. ����������> �)� �{� <��������><�������>�}� | �proc�<�������������>�(�< ������. ����. ����������>�)��{�<�������>�}�
	void FunctionDefine() {
		tree.down("<���������� �������>", stream.getCurrentLine());

		nextLexem("proc");
		nextIdentifier();

		nextLexem("(");
		AcceptedArguments();
		nextLexem(")");

		nextLexem("{");

		store();
		// <��������>
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

	// <������. ����. ����������> = <����. ��������>|< ����. ��������>�,�<������. ����. ����������>
	void AcceptedArguments() {
		tree.down("<������. ����. ����������>", stream.getCurrentLine());

		store();

		// <�������������>�,�<������. �����. ����������>
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

	// <�������> = �return� � � <�������>
	void Return() {
		tree.down("<�������>", stream.getCurrentLine());
		
		nextLexem("return");
		Operand();

		tree.up();
	}

	// <����������> = <�������������>�=�<����������� ��������>
	void Assignment() {
		tree.down("<����������>", stream.getCurrentLine());

		nextIdentifier();
		nextLexem("=");
		Expression();

		tree.up();
	}

	// <����������� ��������> = <�����> | <�������><���. ��������><����������� ��������>|<�������>
	void Expression() {
		tree.down("<����������� ��������>", stream.getCurrentLine());

		store();

		// <�����>
		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <�������><���. ��������><����������� ��������>
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

	// <�������> = <������. ����> | <�������������> | <�����>
	void Operand() {
		tree.down("<�������>", stream.getCurrentLine());

		Token token = stream.get();

		// �������� ��������� 123123
		if (token.get_type() == CONSTANT) {
			tree.up();
			return;
		}

		stream.back();

		store();
		// ����� �������
		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		nextIdentifier();

		tree.up();
	}


	// <�����> = <�������������>�(�<������. �����. ����������>�)�
	void Call() {
		tree.down("<�����>", stream.getCurrentLine());
		nextIdentifier();
		nextLexem("(");
		PassedArguments();
		nextLexem(")");
		tree.up();
	}

	// <������. �����. ����������> = <�������������> | <�������������>�,�<������. �����. ����������>
	void PassedArguments() {
		tree.down("<������. �����. ����������>", stream.getCurrentLine());

		store();

		// <�������������>�,�<������. �����. ����������>
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
		cout << setw(10) << "���" << setw(20) << "�������������" << endl;

		for (auto var : variables)
			cout <<  setw(10) << var.get_type() << setw(20) << var.get_name() << endl;
	}

	void parse() {
		stream.seek(0);
		S();
	}

};

