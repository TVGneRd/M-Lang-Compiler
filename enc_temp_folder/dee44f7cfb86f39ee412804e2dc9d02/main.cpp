#include <locale>
#include <iostream>
#include <string>
#include <conio.h>

#include "scanner.h"
#include "Parser.h"
#include "ParseErrors.h"
#include "Generator.h"

#define ARROW_UP 72
#define ARROW_DOWN 80
#define ARROW_LEFT 75
#define ARROW_RIGTH 77
#define ENTER 13

using namespace std;

int lastSelectedMenuLine = 0;

Scanner scanner;
//Analyzer analyzer;

void backLoop() {
	cout << endl << "Нажмите любую клавишу для выхода в меню";
	_getch();
}

int menu(string status, string* commands, size_t commandsLength) {

	int activeLine = lastSelectedMenuLine;

	while (true) {
		system("@cls||clear");

		cout << status << "Компиляторы. Лабораторная работа №2" << endl << endl;

		for (size_t i = 0; i < commandsLength; i++) {
			if (activeLine == i) cout << "\033[36m";
			cout << i + 1 << ". " << commands[i] << "\033[0m" << endl;
		}

		int command = _getch();
		if (command == 224) command = _getch();

		switch (command) {
		case ARROW_DOWN:
			activeLine = (activeLine >= commandsLength - 1) ? 0 : activeLine + 1;
			break;

		case ARROW_UP:
			activeLine = (activeLine == 0) ? commandsLength - 1 : activeLine - 1;
			break;

		case ARROW_LEFT:
			system("@cls||clear");
			return -1;

		case ARROW_RIGTH:
			system("@cls||clear");
			return -2;

		case ENTER:
			lastSelectedMenuLine = activeLine;
			system("@cls||clear");
			return activeLine;
		}
	}
}

bool checkScanner() {
	if (!scanner.isEmpty()) return true;
	cout << "\033[31mДля начала проведите лексический анализ!\033[0m" << endl;

	return false;
}

void applyCommand(int command) {
	system("@cls||clear");
	switch (command) {

	case 0: {
		scanner.to_scan();
		scanner.print_stream_of_lex();
		backLoop();
		break;
	}

	case 1: {
		if (checkScanner()) {
			scanner.print_table_of_constant();
		}
		backLoop();
		break;
	}

	case 2: {
		if (checkScanner()) {
			scanner.print_table_of_identifier();
		}
		backLoop();
		break;
	}

	case 3: {
		if (checkScanner()) {
			scanner.print_table_of_keyWord();
		}
		backLoop();
		break;
	}

	case 4: {
		if (checkScanner()) {
			scanner.print_table_of_divider();
		}
		backLoop();
		break;
	}
	
	case 5: {
		Scanner scanner;

		scanner.to_scan();

		if (checkScanner()) {

			Table<Token> stream = scanner.getStream();
			Parser parser(&stream);

			try
			{
				cout << endl;
				parser.parse();
				//analyzer.to_analize(&stream, new Table<Variable>(), new Table<Function>());
				cout << "\033[30;42m Синтаксических ошибок не обнаружено.\033[0m" << endl;
			}
			catch (SyntaxError err)
			{
				cout << "\033[30;41mОшибка синтаксиса!\033[0m" << endl;

				cout << err.what() << endl;

			}
			catch (SematnicError err)
			{
				cout << "\033[30;41mСемантическая ошибка!\033[0m" << endl;

				cout << err.what() << endl;

			}
			catch (...)
			{
				cout << "\033[30;41mНеизвестная ошибка!\033[0m" << endl;
			}

			cout << endl << "Дерево:" << endl;
			parser.printTree();

			cout << endl << "Таблица идентификаторов:" << endl;
			parser.printVariablesTable();

		}
		backLoop();
		break;
	}

	case 6: {
		Scanner scanner;

		scanner.to_scan();

		if (checkScanner()) {

			Table<Token> stream = scanner.getStream();
			Parser parser(&stream);

			try
			{
				cout << endl;
				parser.parse();
				//analyzer.to_analize(&stream, new Table<Variable>(), new Table<Function>());
				cout << "\033[30;42m Синтаксических ошибок не обнаружено.\033[0m" << endl;
			}
			catch (SyntaxError err)
			{
				cout << "\033[30;41mОшибка синтаксиса!\033[0m" << endl;

				cout << err.what() << endl;

			}
			catch (SematnicError err)
			{
				cout << "\033[30;41mСемантическая ошибка!\033[0m" << endl;

				cout << err.what() << endl;

			}
			catch (...)
			{
				cout << "\033[30;41mНеизвестная ошибка!\033[0m" << endl;
			}

			Generator generator(parser.getTree());

			generator.genarate();
		}
		backLoop();
		break;
	}

	case 7:
		exit(0);
	}
}

int main() {
	setlocale(LC_ALL, "Rus");

	string commands[] = {
		"\033[32mПровести лексический анализ полностью\033[0m",
		"Посмотреть константы",
		"Посмотреть идентификаторы",
		"Посмотреть ключевые слова",
		"Посмотреть разделители",
		"\033[33mСинтаксический анализ\033[0m",
		"\033[36mИнтерпритация\033[0m",
		"\033[31mВыйти\033[0m",
	};

	int listId = 0;

	for (;;) {
		string status;

		int command = menu(status, commands, sizeof(commands) / sizeof(string));

		switch (command) {

		case -1:
			listId = (listId == 0) ? 2 : listId - 1;
			break;

		case -2:
			listId = (listId >= 2) ? 0 : listId + 1;
			break;

		default:
			applyCommand(command);
		}
	}
}
