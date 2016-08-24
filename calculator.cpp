#include <string>
#include <sstream>
#include <stack>
#include <map>
#include <functional>
#include <math.h>
#include <cmath>
#include <iostream>
using namespace std;
#include "calculator.h"

#define ZERO 1e-15
#define MAX_VALUE 1e+7
#define ACCURACY 100

struct Operator {
	int priority;
	function<string(stack<double>&)> execute;

	Operator() {}

	Operator(int priority, function<string(stack<double>&)> func) {
		this->priority = priority;
		this->execute = func;
	}
};

map <string, Operator> operationMap = {
    { "(", Operator(0,[](stack<double>& operandStack) {
		return "Нарушен порядок скобок: не все скобки закрыты"; })},
	{ "+unary", Operator(9,[](stack<double>& operandStack) {
		if (operandStack.empty()) return "Отсутсвует операнд для унарного +";
		double value1 = operandStack.top();
		operandStack.pop();
		operandStack.push(value1);
		return ""; })},
	{ "-unary", Operator(9,[](stack<double>& operandStack) {
		if (operandStack.empty()) return "Отсутсвует операнд для унарного -";
		double value1 = operandStack.top();
		operandStack.pop();
		operandStack.push(-value1);
		return ""; }) },
	{ "+", Operator(1,[](stack<double>& operandStack) {
		if (operandStack.empty()) return "Отсутсвуют операнды для бинарного +";
		double value1 = operandStack.top();
		operandStack.pop();
		if (operandStack.empty()) return "Отсутсвует второй операнд для бинарного +";
		double value2 = operandStack.top();
		operandStack.pop();
		operandStack.push(value1 + value2);
		return ""; }) },
	{ "-", Operator(1,[](stack<double>& operandStack) {
		if (operandStack.empty()) return "Отсутсвуют операнды для бинарного -";
		double value1 = operandStack.top();
		operandStack.pop();
		if (operandStack.empty()) return "Отсутсвует второй операнд для бинарного -";
		double value2 = operandStack.top();
		operandStack.pop();
		operandStack.push(value2 - value1);
		return ""; }) },
	{ "*", Operator(2,[](stack<double>& operandStack) {
		if (operandStack.empty()) return "Отсутсвуют операнды для *";
		double value1 = operandStack.top();
		operandStack.pop();
		if (operandStack.empty()) return "Отсутсвует второй операнд для *";
		double value2 = operandStack.top();
		operandStack.pop();
		operandStack.push(value2 * value1);
		return ""; }) },
	{ "/", Operator(2,[](stack<double>& operandStack) {
		if (operandStack.empty()) return "Отсутсвуют операнды для /";
		double value1 = operandStack.top();
		if(abs(value1) <= ZERO) return "Деление на 0!";
		operandStack.pop();
		if (operandStack.empty()) return "Отсутсвует второй операнд для /";
		double value2 = operandStack.top();
		operandStack.pop();
		operandStack.push(value2 / value1);
		return ""; }) }
};

void SkipSpaces(string& statement, string::iterator& it) {
	while (it != statement.end() && isspace(*it)) { it++; }
}

bool _isDigit(char c) {
	return c <= '9' && c >= '0';
}

bool _isSeparator(char c) {
	return c == '.' || c == ',';
}


const string ERROR1 = "Некорректный ввод, строка содержит недопустимое выражение ";
const string ERROR2 = "Нарушен порядок скобок: закрывающая скобка обнаружена раньше выражения";
const string ERROR3 = "Нарушен порядок скобок: закрывающая скобка обнаружена раньше открывающей";
const string ERROR4 = "Некорректный ввод, возможно пропущен бинарный оператор";
const string ERROR5 = "Результат вычисления слишком большой по модулю";
const string ERROR6 = "Неверно составленно выражение";


string ExtractOpenParenthesisAndUnaryOperators(string& statement, string::iterator& it, stack<string>& operationStack) {
	string buffer;
	SkipSpaces(statement, it);
	while (it != statement.end() && !_isDigit(*it) && !_isSeparator(*it)) {
		if (*it == ' ') {
			SkipSpaces(statement, it);
			if (buffer != "") {
				it = statement.begin();
				return ERROR1 + buffer;
			}
			buffer = "";
		}
		else {
			buffer += *it;
			if (buffer == "(") {
				operationStack.push("(");
				buffer = "";
			}
			else {
				if (buffer == ")") {
					it = statement.begin();
					return ERROR2;
				}
				else {
					if (operationMap.find(buffer) != operationMap.end()) {
						if (operationMap.find(buffer + "unary") != operationMap.end()) {
							operationStack.push(buffer + "unary");
							buffer = "";
						}
						else {
							operationStack.push(buffer);
							buffer = "";
						}
					}
				}
			}
			it++;
		}
	}
	if (buffer == "") {
		return "";
	}
	else {
		it = statement.begin();
		return ERROR1 + buffer;
	}
}

string ExtractNumber(string& statement, string::iterator& it, stack<double>& operandStack) {
	auto startit = it;
	int integerPart = 0;
	int fractionalPart = 0;
	bool isFindNumber = false;
	SkipSpaces(statement, it);
	while (it != statement.end() && _isDigit(*it)) {
		integerPart = integerPart * 10 + (*it - '0');
		++it;
		isFindNumber = true;
	}
	if (it != statement.end() && _isSeparator(*it)) {
		++it;
		for(size_t i = ACCURACY; i > 0; i /= 10){
            if (it != statement.end() && _isDigit(*it)) {
                fractionalPart += (*it - '0')* i/10;
                ++it;
                isFindNumber = true;
            }
            else{
                i = 0;
            }
		}
	}
	if ((integerPart >= MAX_VALUE) || (it != statement.end() && (_isDigit(*it) || _isSeparator(*it)))) {
		while (it != statement.end() && (_isDigit(*it) || _isSeparator(*it))) { it++; }
		auto endit = it;
		it = statement.begin();
		return  ERROR1 + string(startit, endit);
	}
	double value = ((double)(integerPart * ACCURACY + fractionalPart)) / ACCURACY;
	if (isFindNumber == true) operandStack.push(value);
	return "";
}

string ExtractClosingParenthesis(string& statement, string::iterator& it, stack<string>& operationStack, stack<double>& operandStack) {
	SkipSpaces(statement, it);
	while (it != statement.end() && *it == ')') {
		while (!operationStack.empty() && operationStack.top() != "(") {
			string result = operationMap[operationStack.top()].execute(operandStack);
			operationStack.pop();
			if (!result.empty()) {
				it = statement.begin();
				return result;
			}
		}
		if (operationStack.empty()) {
			it = statement.begin();
			return ERROR3;
		}
		operationStack.pop();
		it++;
		SkipSpaces(statement, it);
	}
	return "";
}

string ExtractBinaryOperator(string& statement, string::iterator& it, stack<string>& operationStack, stack<double>& operandStack) {
	string buffer = "";
	while (it != statement.end()
			&& operationMap.find(buffer) == operationMap.end()
			&& !isspace(*it)
			&& *it != '('
			&& !_isDigit(*it)
            && !_isSeparator(*it)) {
		buffer += *(it++);
	}
	if (!buffer.empty()) {
		if (operationMap.find(buffer) != operationMap.end()) {
			while (!operationStack.empty() && operationMap[operationStack.top()].priority >= operationMap[buffer].priority) {
				string result = operationMap[operationStack.top()].execute(operandStack);
				operationStack.pop();
				if (!result.empty()) {
					it = statement.begin();
					return result;
				}
			}
			operationStack.push(buffer);
			return "";
		}
		else {
			it = statement.begin();
			return ERROR1 + buffer;
		}
		return "";
	}
	else {
		return ERROR4;
	}
}

string ConvertDoubleToFormatString(double answer){
    if(abs(answer) >= MAX_VALUE) return ERROR5;
    if(abs(answer) < 0.5/ACCURACY ) answer = 0;
    answer = (roundf(answer * ACCURACY)) / ACCURACY;
    stringstream result;
    result<<answer;
    return result.str();
}

string Calculate(string statement) {
	string result = "";
	stack<string> operationStack;
	stack<double> operandStack;
	auto it = statement.begin();
	while (result.empty() && it != statement.end()) {
		result = ExtractOpenParenthesisAndUnaryOperators(statement, it, operationStack);
		if (result.empty()) result = ExtractNumber(statement, it, operandStack);
		if (result.empty()) result = ExtractClosingParenthesis(statement, it, operationStack, operandStack);
		if (result.empty()) result = ExtractBinaryOperator(statement, it, operationStack, operandStack);

	}

	if (it != statement.end()) {
		return result;
	}
	while (!operationStack.empty()) {
		result = operationMap[operationStack.top()].execute(operandStack);
		operationStack.pop();
		if (!result.empty()) return result;
	}
	if (operandStack.size() == 1) {
        return ConvertDoubleToFormatString(operandStack.top());
	}
	else {
		return ERROR6;
	}
}

