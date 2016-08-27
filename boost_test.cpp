#define BOOST_TEST_MODULE boost_test_macro_overview
#include <boost/test/included/unit_test.hpp>

#include <time.h>
#include <iostream>
#include <string>
#include <random>
#include <math.h>
#include <cmath>
using namespace std;
#include "calculator.h"


const string OPERATIONS[4] = {"+","-","*","/"};

string SimpleExpressionGenerator(double& Answer, int level = 0){
    int numberOfOperands = 2 + rand()%3;
    stringstream expression;
    int numberOfOperations = 4;
    double currentResult = 0;
    int lastOperation = 0;
    for(size_t i = 0; i < numberOfOperands; i++){
        int operation = rand()%numberOfOperations;
        if(operation < 2){
            numberOfOperations = 2;
        }
        double operand = (rand()%10000)/100.0 - 30;
        if(rand()%2 == 0 && level < 2){
            expression <<"("<<SimpleExpressionGenerator(operand, level+1)<<")";
        }
        else{
            expression << operand;
        }
        expression << OPERATIONS[operation];
        switch(lastOperation){
            case 0:
                currentResult += operand;
                break;
            case 1:
                currentResult -= operand;
                break;
            case 2:
                currentResult *= operand;
                break;
            case 3:
                currentResult /= operand;
                break;
        }
        lastOperation = operation;
    }
    double lastOperand = (rand()%10000 + 1)/100.0;
    switch(lastOperation){
        case 0:
            currentResult += lastOperand;
            break;
        case 1:
            currentResult -= lastOperand;
            break;
        case 2:
            currentResult *= lastOperand;
            break;
        case 3:
            currentResult /= lastOperand;
            break;
        }
    Answer = currentResult;
    expression << lastOperand;
    return expression.str();
}


BOOST_AUTO_TEST_CASE(GeneratorExpressionTest)
{
    setlocale(LC_ALL, "Russian");
    namespace tt = boost::test_tools;
    srand(time(0));
    for(size_t i = 0; i < 1; i++){
        double answer;
        string expression = SimpleExpressionGenerator(answer);
        string actual = Calculate(expression);
        if(actual.compare(string("Деление на 0!")) != 0){
            if(abs(answer) < 10000000.0){
                answer =(roundf(answer * 100)) / 100.0;
                if(abs(answer) < 0.005) answer = 0;
                stringstream expected;
                expected <<answer;
                BOOST_TEST(actual.compare(expected.str()) == 0, expression <<" = " << expected.str() << " vs "<<actual);
            }
            else{
                BOOST_TEST(actual.compare(string("Результат вычисления слишком большой по модулю")) == 0, expression <<" = " << answer <<" vs "<<actual);
            }
        }

    }
}


BOOST_AUTO_TEST_CASE(SomeSpecialTests)
{
    setlocale(LC_ALL, "Russian");
    BOOST_TEST(Calculate("-1 + 5 - 3").compare("1") == 0);
    BOOST_TEST(Calculate("-10 + (8*2.5) - (3/1.5)").compare("8") == 0);
    BOOST_TEST(Calculate("1 + (2 * (2.5 + 2.5 + (3-2)))-(3 / 1.5)").compare("11") == 0);
    BOOST_TEST(Calculate("1.1 + 2.1 + abc").compare("Некорректный ввод, строка содержит недопустимое выражение abc") == 0);
}

BOOST_AUTO_TEST_CASE(SomeSimpleTests)
{
    setlocale(LC_ALL, "Russian");
    BOOST_TEST(Calculate("        1          ").compare("1") == 0);
    BOOST_TEST(Calculate("-        2 + 1  ").compare("-1") == 0);
    BOOST_TEST(Calculate(" 1--- --- -2").compare("-1") == 0);
    BOOST_TEST(Calculate(" 1+++ +++ +2").compare("3") == 0);
    BOOST_TEST(Calculate("80.75*6.9").compare("557.18") == 0);
    BOOST_TEST(Calculate("        0.1786868          ").compare("Некорректный ввод, строка содержит недопустимое выражение 0.1786868") == 0);
    BOOST_TEST(Calculate("  (18827475.17)     ").compare("Некорректный ввод, строка содержит недопустимое выражение 18827475.17") == 0);
    BOOST_TEST(Calculate("(2+2) - 5)").compare("Нарушен порядок скобок: закрывающая скобка обнаружена раньше открывающей") == 0);
    BOOST_TEST(Calculate("(2+2) - (5").compare("Нарушен порядок скобок: не все скобки закрыты") == 0);
    BOOST_TEST(Calculate("(2+2)+()").compare("Нарушен порядок скобок: закрывающая скобка обнаружена раньше выражения") == 0);
    BOOST_TEST(Calculate("").compare("Неверно составленно выражение") == 0);
    BOOST_TEST(Calculate("   ").compare("Неверно составленно выражение") == 0);
    BOOST_TEST(Calculate("4 5").compare("Некорректный ввод, возможно пропущен бинарный оператор") == 0);
    BOOST_TEST(Calculate("+").compare("Отсутсвует операнд для унарного +") == 0);
    BOOST_TEST(Calculate("-").compare("Отсутсвует операнд для унарного -") == 0);
    BOOST_TEST(Calculate("*").compare("Отсутсвуют операнды для *") == 0);
    BOOST_TEST(Calculate("/").compare("Отсутсвуют операнды для /") == 0);
    BOOST_TEST(Calculate("4+").compare("Отсутсвует второй операнд для бинарного +") == 0);
    BOOST_TEST(Calculate("4-").compare("Отсутсвует второй операнд для бинарного -") == 0);
    BOOST_TEST(Calculate("4*").compare("Отсутсвует второй операнд для *") == 0);
    BOOST_TEST(Calculate("4/").compare("Отсутсвует второй операнд для /") == 0);
    BOOST_TEST(Calculate("(2+2)/(2-2)").compare("Деление на 0!") == 0);
}

