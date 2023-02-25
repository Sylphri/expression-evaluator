#pragma once

struct Token {
    union {
        double number;
        char operation;
    };

    enum TokenType {
        NUMBER,
        OPERATION
    } type;

    Token(double number, TokenType type = Token::NUMBER) : number(number), type(type) {}

    Token(char operation, TokenType type = Token::OPERATION)
        : operation(operation), type(type) {}
};
