#pragma once

#include <iostream>

#include <string>
#include <vector>

#include "Token.hpp"

bool isOperation(Token token, char operation)
{
	return token.type == Token::OPERATION && token.operation == operation;
}

bool isNextThatOperation(const std::vector<Token>& tokens, int32_t position, char operation)
{
	return position + 1 < tokens.size() && isOperation(tokens[position + 1], operation);
}

std::string concatTokens(const std::vector<Token> tokens, int32_t highlightToken, int32_t& outPosition)
{
	std::string expression;
	int32_t position = 0;
	for (int32_t i = 0; i < tokens.size(); i++)
	{
		if (i <= highlightToken)
			position = expression.size();
		if (isOperation(tokens[i], ')') && expression.back() != '(')
			expression.pop_back();
		int32_t size = expression.size();
		expression += (tokens[i].type == Token::NUMBER ? std::to_string(tokens[i].number)
			: std::string(1, tokens[i].operation)) + " ";
		if (isOperation(tokens[i], '('))
			expression.pop_back();
	}
	expression.pop_back();
	outPosition = position;
	return expression;
}

void printTokens(const std::vector<Token>& tokens)
{
	std::cout << "Tokens:\n";
	for (Token token : tokens)
	{
		if (token.type == Token::NUMBER)
			std::cout << "Type: number,    Value: " << token.number << std::endl;
		else
			std::cout << "Type: operation, Value: " << token.operation << std::endl;
	}
}

void tokenizerErrorPrint(const std::string& message, const std::string& expression, int32_t errorPosition)
{
	std::string line(expression.size(), '-');
	line[errorPosition] = '^';
	std::cout << expression << std::endl << line << std::endl;
	std::cout << "[Tokenizer Error]: " << message;
}

void verifyerErrorPrint(const std::vector<Token>& tokens, int32_t errorToken, const std::string& message)
{
	int32_t position;
	std::string expression = concatTokens(tokens, errorToken, position);
	std::cout << "[Verifyer Error]: " << message << std::endl;
	std::string line(expression.size(), '-');
	line[position] = '^';
	std::cout << expression << std::endl << line << std::endl;
}