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
	// Âñïîìîãàòåëüíûå ìåòîäû 
	//

	// Âûâîä îøèáêè
	void ErrorOnToken(Token t, string message) {
		throw SyntaxError(message, t.get_line_number());
	}

	// Çàïîìèíàåò ñîñòîÿíèå ïîòîêà/äåðåâà
	void store() {
 		treeStore.push_back(tree);
		streamStore.push_back(stream.getIterrator());
		variablesStore.push_back(variables);
	}

	// Óäàëÿåò ñîñòîíèå ïîòîêà/äåðåâà
	void dropStore() {
		streamStore.pop_back();
		treeStore.pop_back();
		variablesStore.pop_back();
	}

	// Âîñòàíàâëèâàåò ñîñòîÿíèå ïîòîêà/äåðåâà
	void restore() {
		stream.seek(streamStore.back());
		tree.free();
		tree = treeStore.back();
		variables = variablesStore.back();

		dropStore();
	}

	// Äîáàâëåíèå ïåðåìåííîé â òàáëèöó
	void addVariable(string type, string name) {
		Variable var = Variable(name, type);

		if (checkVariableExists(name)) throw SematnicError(format("Èäåíòèôèêàòîð \033[30;35m{}\033[30;0m îïðåäåëåí ðàíåå", name), stream.getCurrentLine());

		variables.push_back(var);
	}

	// Ïðîâåðêà ñóùåñòâîâàíèÿ ïåðìåííîé â òàáëèöå
	bool checkVariableExists(string name) {
		for (auto var : variables) {
			if (var.get_name() == name) return true;
		}

		return false;
	}

	//
	// Âñïîìîãàòåëüíûå ìåòîäû äëÿ ðàáîòû ñ ëåêñåìàìè
	//

	// Ñëåäóþùàÿ ëåêñåìà
	void nextLexem(string name)
	{
		Token token = stream.get();

		if (token != name) ErrorOnToken(token, format("Îæèäàëîñü {}", name));
	}

	// Ñëåäóþùèé òèï
	void nextType()
	{
		tree.down("TYPE: " + stream.getCurrentElement()->get_name(), stream.getCurrentElement()); tree.up();

		Token token = stream.get();

		if (!contains(TYPES, token.get_name())) ErrorOnToken(token, "Îæèäàëñÿ òèï");
	}

	// Ñëåäóþùèé èäåíòèôèêàòîð
	void nextIdentifier()
	{
		tree.down("ID: " + stream.getCurrentElement()->get_name(), stream.getCurrentElement()); tree.up();
		Token token = stream.get();
		
		if (token.get_type() != IDENTIFIER) ErrorOnToken(token, "Îæèäàëñÿ èäåíòèôèêàòîð");
	}

	// Ñëåäóþùèé ìàò. îïåðàòîð
	void nextMathOperator()
	{
		tree.down("Ìàò. îïåðàòîð: " + stream.getCurrentElement()->get_name(), stream.getCurrentElement()); tree.up();
		Token token = stream.get();

		if (!contains(MATH_OPERATORS, token.get_name())) ErrorOnToken(token, "Îæèäàëñÿ ìàò. îïåðàòîð");
	}

	// Ñëåäóþùèé ëîã. îïåðàòîð
	void nextLogicOperator()
	{
		tree.down("Ëîãè÷åñêèé îïåðàòîð: " + stream.getCurrentElement()->get_name(), stream.getCurrentElement()); tree.up();

		Token token = stream.get();

		if (!contains(LOGIC_OPERATORS, token.get_name())) ErrorOnToken(token, "Îæèäàëñÿ ëîãè÷åñêèé îïåðàòîð");
	}

	//
	// Îáðàáîòêà ïðàâèë ãðàììàòèêè
	//

	// <S> = “main” ”(” “)” “;” “{”<ÎÏÅÐÀÖÈÈ>”}”
	void S()
	{
		tree.down("<S>", stream.getCurrentElement());

		nextLexem("main");
		nextLexem("(");
		nextLexem(")");
		nextLexem(";");
		nextLexem("{");
		Operations();
		nextLexem("}");

		tree.up();
	}

	// <ÎÏÅÐÀÖÈÈ> = <ÎÏÅÐÀÖÈß>”;”<ÎÏÅÐÀÖÈÈ>”;” | <ÎÏÅÐÀÖÈß>”;”
	void Operations(){
		tree.down("<ÎÏÅÐÀÖÈÈ>", stream.getCurrentElement());

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

	// <ÎÏÅÐÀÖÈß> = <ÎÁÚßÂËÅÍÈÅ ÏÅÐÅÌÅÍÍÛÕ> | <ÎÁÚßÂËÅÍÈÅ ÔÓÍÊÖÈÈ> | <ÏÐÈÑÂÎÅÍÈÅ> | <ÂÛÇÎÂ> | <ÖÈÊË>
	void Operation() {
		tree.down("<ÎÏÅÐÀÖÈß>", stream.getCurrentElement());
		
		store();

		// <ÎÁÚßÂËÅÍÈÅ ÏÅÐÅÌÅÍÍÛÕ>
		try {
			VariableDefine();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <ÏÐÈÑÂÎÅÍÈÅ>
		try {
			Assignment();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <ÖÈÊË>
		try {
			While();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();
		// <ÂÛÇÎÂ>
		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		// <ÎÁÚßÂËÅÍÈÅ ÔÓÍÊÖÈÈ>
		FunctionDefine();
		
		tree.up();
	}

	// <ÖÈÊË> = “while””(”<ËÎÃÈ×ÅÑÊÀß ÎÏÅÐÀÖÈß>”)”<ÎÏÅÐAÖÈß>”;”
	void While() {
		tree.down("<ÖÈÊË>", stream.getCurrentElement());

		nextLexem("while");
		nextLexem("(");
		LogicExpression();
		nextLexem(")");
		Operation();

		tree.up();
	}

	// <ËÎÃÈ×ÅÑÊÀß ÎÏÅÐÀÖÈß> = <ÎÏÅÐÀÍÄ><ËÎÃÈ×ÅÑÊÈÉ ÎÏÅÐÀÒÎÐ><ÎÏÅÐÀÍÄ>
	void LogicExpression() {
		tree.down("<ËÎÃÈ×ÅÑÊÀß ÎÏÅÐÀÖÈß>", stream.getCurrentElement());

		Operand();
		nextLogicOperator();
		Operand();

		tree.up();
	}

	// <ÎÁÚßÂËÅÍÈÅ ÏÅÐÅÌÅÍÍÛÕ> = <ÒÈÏ><ÈÄÅÍÒÈÔÈÊÀÒÎÐ>
	void VariableDefine() {
		tree.down("<ÎÁÚßÂËÅÍÈÅ ÏÅÐÅÌÅÍÍÎÉ>", stream.getCurrentElement());

		Token type = stream.get();
		Token identifyer = stream.get();
		stream.back(2);

		nextType();
		nextIdentifier();

		addVariable(type.get_name(), identifyer.get_name());

		tree.up();
	}

	// <ÎÁÚßÂËÅÍÈÅ ÔÓÍÊÖÈÈ> = “proc”<ÈÄÅÍÒÈÔÈÊÀÒÎÐ>”(”< ÏÎÑËÅÄ. ÏÐÈÍ. ÀÐÃÓÌÅÍÒÎÂ> ”)” ”{“ <ÎÏÅÐÀÖÈÈ><ÂÎÇÂÐÀÒ>”}” | “proc”<ÈÄÅÍÒÈÔÈÊÀÒÎÐ>”(”< ÏÎÑËÅÄ. ÏÐÈÍ. ÀÐÃÓÌÅÍÒÎÂ>”)””{“<ÂÎÇÂÐÀÒ>”}”
	void FunctionDefine() {
		tree.down("<ÎÁÚßÂËÅÍÈÅ ÔÓÍÊÖÈÈ>", stream.getCurrentElement());

		nextLexem("proc");
		nextIdentifier();

		nextLexem("(");
		AcceptedArguments();
		nextLexem(")");

		nextLexem("{");

		store();
		// <ÎÏÅÐÀÖÈÈ>
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

	// <ÏÎÑËÅÄ. ÏÐÈÍ. ÀÐÃÓÌÅÍÒÎÂ> = <ÏÐÈÍ. ÀÐÃÓÌÅÍÒ>|< ÏÐÈÍ. ÀÐÃÓÌÅÍÒ>”,”<ÏÎÑËÅÄ. ÏÐÈÍ. ÀÐÃÓÌÅÍÒÎÂ>
	void AcceptedArguments() {
		tree.down("<ÏÎÑËÅÄ. ÏÐÈÍ. ÀÐÃÓÌÅÍÒÎÂ>", stream.getCurrentElement());

		store();

		// <ÈÄÅÍÒÈÔÈÊÀÒÎÐ>”,”<ÏÎÑËÅÄ. ÏÅÐÅÄ. ÀÐÃÓÌÅÍÒÎÂ>
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

	// <ÂÎÇÂÐÀÒ> = “return” ” “ <ÎÏÅÐÀÍÄ>
	void Return() {
		tree.down("<ÂÎÇÂÐÀÒ>", stream.getCurrentElement());
		
		nextLexem("return");
		Operand();

		tree.up();
	}

	// <ÏÐÈÑÂÎÅÍÈÅ> = <ÈÄÅÍÒÈÔÈÊÀÒÎÐ>”=”<ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>
	void Assignment() {
		tree.down("<ÏÐÈÑÂÎÅÍÈÅ>", stream.getCurrentElement());

		nextIdentifier();
		nextLexem("=");
		Expression();

		tree.up();
	}

	// <ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ> = <ÎÏÅÐÀÍÄ><ÌÀÒ. ÎÏÅÐÀÒÎÐ><ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>|<ÎÏÅÐÀÍÄ>
	void Expression() {
		tree.down("<ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>", stream.getCurrentElement());

		store();

		// <ÎÏÅÐÀÍÄ><ÌÀÒ. ÎÏÅÐÀÒÎÐ><ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>
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

	// <ÎÏÅÐÀÍÄ> = <ÏÎÑËÅÄ. ÖÈÔÐ> | <ÈÄÅÍÒÈÔÈÊÀÒÎÐ> | <ÂÛÇÎÂ>
	void Operand() {
		tree.down("<ÎÏÅÐÀÍÄ>", stream.getCurrentElement());

		Token token = stream.get();

		// ×èñëîâàÿ êîíñòàíòà 123123
		if (token.get_type() == CONSTANT) {
			tree.up();
			return;
		}

		stream.back();

		store();
		// Âûçîâ ôóíêöèè
		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		nextIdentifier();

		tree.up();
	}


	// <ÂÛÇÎÂ> = <ÈÄÅÍÒÈÔÈÊÀÒÎÐ>”(”<ÏÎÑËÅÄ. ÏÅÐÅÄ. ÀÐÃÓÌÅÍÒÎÂ>”)”
	void Call() {
		tree.down("<ÂÛÇÎÂ>", stream.getCurrentElement());
		nextIdentifier();
		nextLexem("(");
		PassedArguments();
		nextLexem(")");
		tree.up();
	}

	// <ÏÎÑËÅÄ. ÏÅÐÅÄ. ÀÐÃÓÌÅÍÒÎÂ> = <ÈÄÅÍÒÈÔÈÊÀÒÎÐ> | <ÈÄÅÍÒÈÔÈÊÀÒÎÐ>”,”<ÏÎÑËÅÄ. ÏÅÐÅÄ. ÀÐÃÓÌÅÍÒÎÂ>
	void PassedArguments() {
		tree.down("<ÏÎÑËÅÄ. ÏÅÐÅÄ. ÀÐÃÓÌÅÍÒÎÂ>", stream.getCurrentElement());

		store();

		// <ÈÄÅÍÒÈÔÈÊÀÒÎÐ>”,”<ÏÎÑËÅÄ. ÏÅÐÅÄ. ÀÐÃÓÌÅÍÒÎÂ>
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

	Tree* getTree() {
		return &this->tree;
	}

	void printTree() {
		tree.print();
	}

	void printVariablesTable() {
		cout << setw(10) << "Òèï" << setw(20) << "Èäåíòèôèêàòîð" << endl;

		for (auto var : variables)
			cout <<  setw(10) << var.get_type() << setw(20) << var.get_name() << endl;
	}

	void parse() {
		stream.seek(0);
		S();
	}

};

