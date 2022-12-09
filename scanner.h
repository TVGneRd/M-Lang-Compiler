#pragma once
#include "AllStruct.h"
#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;
class Scanner {
	ifstream in_main;//Указатель на файл с иходным кодом

	All_KeyWords table_of_keyWords; //Все ключевые слова, оператор table_of_keyWords == string obg возвращает true, если в таблице есть такое слово
	All_Dividers table_of_dividers; //Все разделители, оператор table_of_dividers == string obg возвращает true, если в таблице есть такой разделитель

	Table <Token> stream_of_token; //Таблица потока лексем(обращение stream_of_token[i]). Оператор stream_of_token += Token добавляет в таблицу токен
	Table <Identifier> table_of_identifier; //Таблица ID(обращение table_of_identifier[i]). Оператор table_of_identifier += Identifier добавляет в таблицу токен
	Table <Constant> table_of_constant; //Таблица констант. (обращение table_of_constant[i]). Оператор table_of_constant += Constant добавляет в таблицу константу 
	Table <KeyWord> keyWords;
	Table <Divider> dividers;

	char keeper_last_char; //Символ, хранящий последний обработаный символ функции get_lex

	bool delete_coment(char c1, char c2);//Удаление комментариев, если коментарий был удалён возвращает true, иначе false
	int lineCounter = 0;

public:
	Scanner();//Получение объекта-указателя на файл, открытие файла
	~Scanner();// Закрытие файла
	string get_lex();//Получение очередной лексемы

	void to_scan();

	void print_stream_of_lex();
	void print_table_of_constant();
	void print_table_of_identifier();
	void print_table_of_keyWord();
	void print_table_of_divider();

	bool isEmpty();

	Table <Token> getStream();
};
