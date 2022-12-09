#include "scanner.h"
Scanner::Scanner() {
	keeper_last_char = '\0';
	in_main.open("main.txt", ios::in);
	if (!in_main.is_open()) {
		cout << "Не удалось открыть: main.txt" << endl;
		system("pause");
		system("cls");
	}
}
Scanner::~Scanner() {
	in_main.close();
}
string Scanner::get_lex() {
	int streamStore = BEGIN_STATE;
	string bufer, t_str;
	char c1, c2 = '\0';
	int err = 0;
	//bool flag = true;
	c1 = keeper_last_char;
	//Пропуск пробелов табуляций и переносов строки преред лексемой
	do {
		if ((c1 == ' ') || (c1 == '\t') || (c1 == '\n')) {
			if (c1 == '\n') {
				lineCounter++;
			}
			//flag = false;
			if (!in_main.eof())
				c1 = in_main.get();//Получение символа char
		}
		else {
			break;
		}
	} while (!in_main.eof());
	if (keeper_last_char == '\0') {
		if (!in_main.eof())
			c1 = in_main.get();//Получение символа char
	}

	if (!in_main.eof())
		c2 = in_main.get();//Получение символа char
	if ((c1 >= '0') && (c1 <= '9'))
		streamStore = CONSTANT;

	while (((!in_main.eof()) || (c1 > 0)) && c2) {
		//Начало коментария, если встречен с1 =/ с2 =/ Или с1 =/ с2 = *
		if (delete_coment(c1, c2)) {
			//Пропуск коментариев
			//flag = true;
			if (!in_main.eof())
				c1 = in_main.get();
			//Пропуск пробелов табуляций и переносов строки преред лексемой после пропуска коментариев
				while (!in_main.eof()) {
					if ((c1 == ' ') || (c1 == '\t') || (c1 == '\n')) {
						//flag = false;
						if (c1 == '\n') {
							lineCounter++;
						}
						if (!in_main.eof())
							c1 = in_main.get();//Получение символа char
					}
					else {
						break;
					}
				}
			if (!in_main.eof())
				c2 = in_main.get();//Получение символа char2
			if ((c1 >= '0') && (c1 <= '9'))
				streamStore = CONSTANT;
		}
		//получение лексемы
		t_str = c1;
		if (streamStore == CONSTANT) {
			if (((c1 >= 'A') && (c1 <= 'Z')) || ((c1 >= 'a') && (c1 <= 'z'))) {
				t_str = "";
				err++;
				Token scanner(t_str, ERROR, lineCounter);
				stream_of_token += scanner;
				cout << "\033[30;41m Ошибка! Программа провела лексический анализ до строки, в которой она допущена.\033[0m" << endl;
				return t_str;
			}
		}
		else {
			if (!(((c1 >= 'A') && (c1 <= 'Z')) || ((c1 >= 'a') && (c1 <= 'z')) || (table_of_dividers == t_str) || ((c1 >= '0') && (c1 <= '9')))) {
				t_str = "";
				err++;
				Token scanner(t_str, ERROR, lineCounter);
				stream_of_token += scanner;
				cout << "\033[30;41m Ошибка! Программа провела лексический анализ до строки, в которой она допущена.\033[0m" << endl;
				return t_str;
			}
		}
		bufer += c1;
		t_str = c2;
		
		//Если с1 разделитель или с2 разделитель или пробел илитабуляция или перенос строки получить лексему
			if (table_of_dividers == bufer) {
				keeper_last_char = c2;
				//Разделитель
				Token scanner(bufer, DIVIDER, lineCounter);
				stream_of_token += scanner;
				Divider B(bufer);
				dividers += B;
				return bufer;
			}
		if ((table_of_dividers == t_str) || (c2 == ' ') || (c2 == '\t') || (c2 == '\n')) {
			keeper_last_char = c2;
			//Константа или ID Или Ключевое слово
			
			if (streamStore == CONSTANT) {
				//Константа
				Token scanner(bufer, CONSTANT, lineCounter);
				stream_of_token += scanner;

				if (!(table_of_constant == bufer)) {
					Constant B(bufer);
					table_of_constant += B;
				}
			}
			else {
				//Ключевое слово
				if (table_of_keyWords == bufer) {
					Token scanner(bufer, KEYWORD, lineCounter);
					stream_of_token += scanner;
					KeyWord B(bufer);
					keyWords += B;
				}
				//Идентификатор
				else {
					Token scanner(bufer, IDENTIFIER, lineCounter);
					stream_of_token += scanner;

					if (!(table_of_identifier == bufer)) {
						Identifier B(bufer);
						table_of_identifier += B;
					}
				}
			}
			return bufer;
		}
		c1 = c2;
		if (!in_main.eof())
			c2 = in_main.get();
	}
	bufer = "";//При возвращении пустой строки - конечное состояние
	Token scanner(bufer, END_OF_PROGRAMM, lineCounter);
	stream_of_token += scanner;
	if (err == 0)
		cout << "\033[30;42m   Лексических ошибок в программе не обнаружено   \033[0m" << endl;
	return bufer;
}
bool Scanner::delete_coment(char c1, char c2) {
	if (c1 == '/') {
		if (c2 == '/') {
			//Однострочный коментарий
			char c1;
			do {
				if (!in_main.eof())
					c1 = in_main.get();
			} while (c1 != '\n');

			lineCounter++;

			return true;
		}
		else {
			//многострочный коментарий
			if (c2 == '*') {
				while (!in_main.eof()) {
					if (!in_main.eof())
						c1 = in_main.get();
						if (c1 == '\n') {
							lineCounter++;
						}
					if (c1 == '*') {
						if (in_main.eof())
							return false;
						if (!in_main.eof())
							c2 = in_main.get();
						if (c2 == '/')
							return true;
					}
				}
			}
		}
	}
	return false;
}

void Scanner::to_scan() {
	lineCounter = 1;

	while (get_lex() != "") {}

}
void Scanner::print_stream_of_lex() {
	string type;
	cout << setw(10) << "№" << setw(10) << "Строка" << setw(20) << "Лексема" << setw(20) << " Тип лексемы" << endl;
	for (int i = 0; i < stream_of_token.get_count() - 1; i++) {
		switch (stream_of_token[i].get_type()) {
		case KEYWORD:
			type = "\033[31m      КЛЮЧЕВОЕ СЛОВО\033[0m";
			break;
		case DIVIDER:
			type = "РАЗДЕЛИТЕЛЬ";
			break;
		case IDENTIFIER:
			type = "\033[36m       ИДЕНТИФИКАТОР\033[0m";
			break;
		case CONSTANT:
			type = "\033[35m           КОНСТАНТА\033[0m";
			break;
		}
		cout << setw(10) << i + 1 << setw(10) << stream_of_token[i].get_line_number() << setw(20) << stream_of_token[i].get_name() << setw(20) << type << endl;
	}

}
void Scanner::print_table_of_constant() {
	int i = 0;
	//Вывод таблицы констатнт
	cout << setw(20) << "№ Константы" << setw(30) << " Значение константы" << endl;
	for (i = 0; i < table_of_constant.get_count(); i++)
		cout << setw(10) << i + 1 << setw(40) << table_of_constant[i].get_name() << endl;
}

void Scanner::print_table_of_identifier() {
	int i = 0;
	//Вывод таблицы идинтефикаторов
	cout << setw(25) << "№ Идентификатора" << setw(25) << "Идентификатор" << endl;
	for (i = 0; i < table_of_identifier.get_count(); i++)
		cout << setw(10) << i + 1 << setw(40) << table_of_identifier[i].get_name() << endl;
}

void Scanner::print_table_of_keyWord() {
	int i = 0;
	//Вывод таблицы констатнт
	cout << setw(26) << "№ Ключевого слова" << setw(24) << "Ключевое слово" << endl;
	for (i = 0; i < keyWords.get_count(); i++)
		cout << setw(10) << i + 1 << setw(40) << keyWords[i].get_name() << endl;
}

void Scanner::print_table_of_divider() {
	int i = 0;
	//Вывод таблицы констатнт
	cout << setw(22) << "№ Разделителя" << setw(28) << "Разделитель" << endl;
	for (i = 0; i < dividers.get_count(); i++)
		cout << setw(10) << i + 1 << setw(40) << dividers[i].get_name() << endl;
}

bool Scanner::isEmpty()
{
	return stream_of_token.get_count() == 0;
}

Table<Token> Scanner::getStream()
{
	return stream_of_token;
}
