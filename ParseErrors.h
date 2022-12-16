#pragma once

#include <algorithm>
#include <string>
#include <format>

using namespace std;

class SyntaxError : public std::exception {
private:
    string message;
    int line;

public:
    SyntaxError(string msg, int num) {
        message = msg;
        line = num;
    }

    string what() {
        return format("Синтаксическая ошибка: {}\nСтрока: \033[30;36m{}\033[30;0m", message, line);
    }
};

class SematnicError : public std::exception {
private:
    string message;
    int line;

public:
    SematnicError(string msg, int num) {
        message = msg;
        line = num;
    }

    string what() {
        return format("Семантическая ошибка: {}\nСтрока: \033[30;36m{}\033[30;0m", message, line);
    }
};

class InterpritationError : public std::exception {
private:
    string message;
    int line;

public:
    InterpritationError(string msg) {
        message = msg;
    }

    string what() {
        return format("Интерпретационная ошибка: {}\n", message);
    }
};