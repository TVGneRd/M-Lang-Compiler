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

	// Çàïîìèíàåò ñîñòîÿíèå ïîòîêà/äåğåâà
	void store() {
 		treeStore.push_back(tree);
		streamStore.push_back(stream.getIterrator());
		variablesStore.push_back(variables);
	}

	// Óäàëÿåò ñîñòîíèå ïîòîêà/äåğåâà
	void dropStore() {
		streamStore.pop_back();
		treeStore.pop_back();
		variablesStore.pop_back();
	}

	// Âîñòàíàâëèâàåò ñîñòîÿíèå ïîòîêà/äåğåâà
	void restore() {
		stream.seek(streamStore.back());
		tree.free();
		tree = treeStore.back();
		variables = variablesStore.back();

		dropStore();
	}

	// Äîáàâëåíèå ïåğåìåííîé â òàáëèöó
	void addVariable(string type, string name) {
		Variable var = Variable(name, type);

		if (checkVariableExists(name)) throw SematnicError(format("Èäåíòèôèêàòîğ \033[30;35m{}\033[30;0m îïğåäåëåí ğàíåå", name), stream.getCurrentLine());

		variables.push_back(var);
	}

	// Ïğîâåğêà ñóùåñòâîâàíèÿ ïåğìåííîé â òàáëèöå
	bool checkVariableExists(string name) {
		for (auto var : variables) {
			if (var.get_name() == name) return true;
		}

		return false;
	}

	//
	// Âñïîìîãàòåëüíûå ìåòîäû äëÿ ğàáîòû ñ ëåêñåìàìè
	//

	// Ñëåäóşùàÿ ëåêñåìà
	void nextLexem(string name)
	{
		Token token = stream.get();
		if (token != name) ErrorOnToken(token, format("Îæèäàëîñü {}", name));
	}

	// Ñëåäóşùèé òèï ëåêñåìû
	void nextLexemType(int lexemType)
	{
		Token token = stream.get();
		string lexemTypesName[] = {"Èäåíòèôèêàòîğ", "Êëş÷åâîå ñëîâî", "Ğàçäåëèòåëü", "×èñëîâàÿ êîíñòàíòà"};
		if (token.get_type() != lexemType) ErrorOnToken(token, format("Îæèäàëîñü {}", lexemTypesName[lexemType]));
	}

	// Ñëåäóşùèé òèï
	void nextType()
	{
		Token token = stream.get();
		if (!contains(TYPES, token.get_name())) ErrorOnToken(token, "Îæèäàëñÿ òèï");
	}

	// Ñëåäóşùèé èäåíòèôèêàòîğ
	void nextIdentifier()
	{
		Token token = stream.get();
		if (token.get_type() != IDENTIFIER) ErrorOnToken(token, "Îæèäàëñÿ èäåíòèôèêàòîğ");
	}

	// Ñëåäóşùèé ìàò. îïåğàòîğ
	void nextMathOperator()
	{
		Token token = stream.get();
		if (!contains(MATH_OPERATORS, token.get_name())) ErrorOnToken(token, "Îæèäàëñÿ ìàò. îïåğàòîğ");
	}

	// Ñëåäóşùèé ëîã. îïåğàòîğ
	void nextLogicOperator()
	{
		Token token = stream.get();
		if (!contains(LOGIC_OPERATORS, token.get_name())) ErrorOnToken(token, "Îæèäàëñÿ ëîãè÷åñêèé îïåğàòîğ");
	}

	//
	// Îáğàáîòêà ïğàâèë ãğàììàòèêè
	//

	// <S> = “main” ”(” “)” “;” “{”<ÎÏÅĞÀÖÈÈ>”}”
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

	// <ÎÏÅĞÀÖÈÈ> = <ÎÏÅĞÀÖÈß>”;”<ÎÏÅĞÀÖÈÈ>”;” | <ÎÏÅĞÀÖÈß>”;”
	void Operations(){
		tree.down("<ÎÏÅĞÀÖÈÈ>", stream.getCurrentLine());

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

	// <ÎÏÅĞÀÖÈß> = <ÎÁÚßÂËÅÍÈÅ ÏÅĞÅÌÅÍÍÛÕ> | <ÎÁÚßÂËÅÍÈÅ ÔÓÍÊÖÈÈ> | <ÏĞÈÑÂÎÅÍÈÅ> | <ÂÛÇÎÂ> | <ÖÈÊË>
	void Operation() {
		tree.down("<ÎÏÅĞÀÖÈß>", stream.getCurrentLine());
		
		store();

		// <ÎÁÚßÂËÅÍÈÅ ÏÅĞÅÌÅÍÍÛÕ>
		try {
			VariableDefine();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <ÏĞÈÑÂÎÅÍÈÅ>
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

	// <ÖÈÊË> = “while””(”<ËÎÃÈ×ÅÑÊÀß ÎÏÅĞÀÖÈß>”)”<ÎÏÅĞAÖÈß>”;”
	void While() {
		tree.down("<ÖÈÊË>", stream.getCurrentLine());

		nextLexem("while");
		nextLexem("(");
		LogicExpression();
		nextLexem(")");
		Operation();

		tree.up();
	}

	// <ËÎÃÈ×ÅÑÊÀß ÎÏÅĞÀÖÈß> = <ÎÏÅĞÀÍÄ><ËÎÃÈ×ÅÑÊÈÉ ÎÏÅĞÀÒÎĞ><ÎÏÅĞÀÍÄ>
	void LogicExpression() {
		tree.down("<ËÎÃÈ×ÅÑÊÀß ÎÏÅĞÀÖÈß>", stream.getCurrentLine());

		Operand();
		nextLogicOperator();
		Operand();

		tree.up();
	}

	// <ÎÁÚßÂËÅÍÈÅ ÏÅĞÅÌÅÍÍÛÕ> = <ÒÈÏ><ÈÄÅÍÒÈÔÈÊÀÒÎĞ>
	void VariableDefine() {
		tree.down("<ÎÁÚßÂËÅÍÈÅ ÏÅĞÅÌÅÍÍÎÉ>", stream.getCurrentLine());

		Token type = stream.get();
		Token identifyer = stream.get();
		stream.back(2);

		nextType();
		nextIdentifier();

		addVariable(type.get_name(), identifyer.get_name());

		tree.up();
	}

	// <ÎÁÚßÂËÅÍÈÅ ÔÓÍÊÖÈÈ> = “proc”<ÈÄÅÍÒÈÔÈÊÀÒÎĞ>”(”< ÏÎÑËÅÄ. ÏĞÈÍ. ÀĞÃÓÌÅÍÒÎÂ> ”)” ”{“ <ÎÏÅĞÀÖÈÈ><ÂÎÇÂĞÀÒ>”}” | “proc”<ÈÄÅÍÒÈÔÈÊÀÒÎĞ>”(”< ÏÎÑËÅÄ. ÏĞÈÍ. ÀĞÃÓÌÅÍÒÎÂ>”)””{“<ÂÎÇÂĞÀÒ>”}”
	void FunctionDefine() {
		tree.down("<ÎÁÚßÂËÅÍÈÅ ÔÓÍÊÖÈÈ>", stream.getCurrentLine());

		nextLexem("proc");
		nextIdentifier();

		nextLexem("(");
		AcceptedArguments();
		nextLexem(")");

		nextLexem("{");

		store();
		// <ÎÏÅĞÀÖÈÈ>
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

	// <ÏÎÑËÅÄ. ÏĞÈÍ. ÀĞÃÓÌÅÍÒÎÂ> = <ÏĞÈÍ. ÀĞÃÓÌÅÍÒ>|< ÏĞÈÍ. ÀĞÃÓÌÅÍÒ>”,”<ÏÎÑËÅÄ. ÏĞÈÍ. ÀĞÃÓÌÅÍÒÎÂ>
	void AcceptedArguments() {
		tree.down("<ÏÎÑËÅÄ. ÏĞÈÍ. ÀĞÃÓÌÅÍÒÎÂ>", stream.getCurrentLine());

		store();

		// <ÈÄÅÍÒÈÔÈÊÀÒÎĞ>”,”<ÏÎÑËÅÄ. ÏÅĞÅÄ. ÀĞÃÓÌÅÍÒÎÂ>
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

	// <ÂÎÇÂĞÀÒ> = “return” ” “ <ÎÏÅĞÀÍÄ>
	void Return() {
		tree.down("<ÂÎÇÂĞÀÒ>", stream.getCurrentLine());
		
		nextLexem("return");
		Operand();

		tree.up();
	}

	// <ÏĞÈÑÂÎÅÍÈÅ> = <ÈÄÅÍÒÈÔÈÊÀÒÎĞ>”=”<ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>
	void Assignment() {
		tree.down("<ÏĞÈÑÂÎÅÍÈÅ>", stream.getCurrentLine());

		nextIdentifier();
		nextLexem("=");
		Expression();

		tree.up();
	}

	// <ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ> = <ÂÛÇÎÂ> | <ÎÏÅĞÀÍÄ><ÌÀÒ. ÎÏÅĞÀÒÎĞ><ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>|<ÎÏÅĞÀÍÄ>
	void Expression() {
		tree.down("<ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>", stream.getCurrentLine());

		store();

		// <ÂÛÇÎÂ>
		try {
			Call();

			tree.up();
			return dropStore();
		}
		catch (SyntaxError err) { restore(); };

		store();

		// <ÎÏÅĞÀÍÄ><ÌÀÒ. ÎÏÅĞÀÒÎĞ><ÂÛ×ÈÑËßÅÌÎÅ ÇÍÀ×ÅÍÈÅ>
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

	// <ÎÏÅĞÀÍÄ> = <ÏÎÑËÅÄ. ÖÈÔĞ> | <ÈÄÅÍÒÈÔÈÊÀÒÎĞ> | <ÂÛÇÎÂ>
	void Operand() {
		tree.down("<ÎÏÅĞÀÍÄ>", stream.getCurrentLine());

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


	// <ÂÛÇÎÂ> = <ÈÄÅÍÒÈÔÈÊÀÒÎĞ>”(”<ÏÎÑËÅÄ. ÏÅĞÅÄ. ÀĞÃÓÌÅÍÒÎÂ>”)”
	void Call() {
		tree.down("<ÂÛÇÎÂ>", stream.getCurrentLine());
		nextIdentifier();
		nextLexem("(");
		PassedArguments();
		nextLexem(")");
		tree.up();
	}

	// <ÏÎÑËÅÄ. ÏÅĞÅÄ. ÀĞÃÓÌÅÍÒÎÂ> = <ÈÄÅÍÒÈÔÈÊÀÒÎĞ> | <ÈÄÅÍÒÈÔÈÊÀÒÎĞ>”,”<ÏÎÑËÅÄ. ÏÅĞÅÄ. ÀĞÃÓÌÅÍÒÎÂ>
	void PassedArguments() {
		tree.down("<ÏÎÑËÅÄ. ÏÅĞÅÄ. ÀĞÃÓÌÅÍÒÎÂ>", stream.getCurrentLine());

		store();

		// <ÈÄÅÍÒÈÔÈÊÀÒÎĞ>”,”<ÏÎÑËÅÄ. ÏÅĞÅÄ. ÀĞÃÓÌÅÍÒÎÂ>
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
		cout << setw(10) << "Òèï" << setw(20) << "Èäåíòèôèêàòîğ" << endl;

		for (auto var : variables)
			cout <<  setw(10) << var.get_type() << setw(20) << var.get_name() << endl;
	}

	void parse() {
		stream.seek(0);
		S();
	}

};

