#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

enum { KEYWORD, DIVIDER, IDENTIFIER, CONSTANT, END_OF_PROGRAMM, ERROR, BEGIN_STATE };

class Token {
	//1 Лексема
	string name;
	int type; // 1 - ID; 2 - KEYWORD; 3 - DIVIDER; 4 - CONSTANT; 
	int lineNumber;
public:
	Token(string NAME, int TYPE, int line);
	Token(const Token& obg);
	void set_name(string& N) { name = N; };
	string get_name() { return name; };

	void set_type(int N) { type = N; };
	int get_type() { return type; };
	int get_line_number() { return lineNumber; };

	inline bool operator==(string& obg);
	inline bool operator==(const char* obg);
	inline bool operator!=(const char* obg);

};

inline bool Token::operator!=(const char* obg) {
	return string(obg) != get_name();
}

inline bool Token::operator==(const char* obg) {
	return string(obg) == get_name();
}

inline bool Token::operator==(string& obg) {
	return obg == get_name();
}

class Identifier {
	string name;
public:
	Identifier(string NAME) { name = NAME; };
	Identifier(const Identifier& obg) { name = obg.name; };
	void set_name(string& N) { name = N; };
	string& get_name() { return name; };
};



class Constant {
	string name;
public:
	Constant(string& NAME) { name = NAME; };
	Constant(const Constant& obg) { name = obg.name; };
	void set_name(string& N) { name = N; };
	string& get_name() { return name; };
};
class KeyWord {
	string name;
public:
	KeyWord(string& NAME) { name = NAME; };
	KeyWord(const KeyWord& obg) { name = obg.name; };
	void set_name(string& N) { name = N; };
	string& get_name() { return name; };
};
class Divider {
	string name;
public:
	Divider(string& NAME) { name = NAME; };
	Divider(const Divider& obg) { name = obg.name; };
	void set_name(string& N) { name = N; };
	string& get_name() { return name; };
};


class Function {
	string name;
	vector<string> arguments;
public:
	Function(string& NAME, vector<string> &args) { 
		name = NAME; 
		arguments = args; 
	};
	Function(const Function& obg) { name = obg.name; };
	void set_name(string& N) { name = N; };
	string& get_name() { return name; };
};


class Variable {
	string name;
	string type;
public:
	Variable(string name, string type) { this->name = name; this->type = type; };
	Variable(const Variable& obg) { name = obg.name; type = obg.type; };
	void set_name(string& N) { name = N; };
	string& get_name() { return name; };
	string& get_type() { return type; };
};


class All_KeyWords {
	//Все ключевые слова из входного файла
	vector <string> keyWords;
public:
	All_KeyWords();
	vector <string>& get_vect_key_word() { return keyWords; };
	bool operator ==(string& obg);//Есть ли среди всех ключевых слов слово obg?
};
class All_Dividers {
	//Все ключевые слова из входного файла
	vector <string> dividers;
public:
	All_Dividers();
	vector <string>& get_vect_dividers_word() { return dividers; };
	bool operator ==(string& obg);//Есть ли среди всех разделителей слово obg?
};
template <typename T>
class Table {
	vector <T> table;
	int count;
	int iterrator = 0;
public:

	Table();
	int get_count() { return count; };
	int getIterrator() { return iterrator; };
	int getCurrentLine() { return table[iterrator].get_line_number(); };
	Token* getCurrentElement() { return &table[iterrator]; };

	void seek(int pos);
	void back(int to = 1) { iterrator = max(0, iterrator - to); };

	int indexOf(string name);
	T& get();

	Table <T>& operator +=(T& obg);
	T& operator [](int num);
	Table<T> operator+(const Table<T>& b) const;
	bool operator ==(string& obg);
	
};

template<typename T>
void Table<T>::seek(int pos) {
	iterrator = pos;
}

template<typename T>
int Table<T>::indexOf(string name) {
	for (int i = 0; i < get_count() - 1; i++) 
	{
		if (table[i].get_name() == name) {
			return i;
		}
	}

	return -1;
}

template<typename T>
T& Table<T>::get()  {
	int max = this->get_count() - 1;

	if (this->iterrator > max){
		throw runtime_error("Неожиданный конец программы");
	}

	return this->table[iterrator++];
}

template<typename T>
inline Table<T>::Table() {
	count = 0;
}
template<typename T>
inline Table<T>& Table<T>::operator+=(T& obg) {
	table.push_back(obg);
	count++;
	return *this;
}
template<typename T>
inline bool Table<T>::operator==(string& obg) {
	for (auto var : table)
	{
		if (obg == var.get_name())
			return true;
	}
	return false;
}
template<typename T>
inline T& Table<T>::operator[](int num) {
	return table[num];
}

template<typename T>
Table<T> operator+( Table<T> &b,  Table<T>& a)  {
	Table<T> newTable;

	for (int i = 0; i < a.get_count() - 1; i++) {
		newTable += a[i];
	}

	for (int i = 0; i < b.get_count() - 1; i++) {
		newTable += b[i];
	}

	return newTable;
}