#include <cstdint>

#include <string>
#include <vector>

#include <sstream>

#include "Token.hpp"
#include "Utilities.hpp"

int32_t getIntegerPart(const std::string& expression, int32_t begin, int32_t& outEnd)
{
	int32_t number = 0;
	int32_t i = begin;
	for (; i < expression.size(); i++)
	{
		if (!std::isdigit(expression[i]))
			break;

		number *= 10;
		number += expression[i] - '0';
	}
	outEnd = i - 1;
	return number;
}

double getFractionalPart(const std::string& expression, int32_t begin, int32_t& outEnd)
{
	double fraction = 0;
	double koefficient = 0.1;
	int32_t i = begin;
	for (; i < expression.size(); i++)
	{
		if (!std::isdigit(expression[i]))
			break;

		fraction += (expression[i] - '0') * koefficient;
		koefficient *= 0.1;
	}
	outEnd = i - 1;
	return fraction;
}

bool isNoneDigitsAround(const std::string& expression, int32_t position)
{
	return (position - 1 < 0 || !std::isdigit(expression[position - 1]))
		&& (position + 1 >= expression.size() || !std::isdigit(expression[position - 1]));
}

bool isDigitOrDot(const std::string& expression, int32_t position)
{
	return std::isdigit(expression[position]) || expression[position] == '.';
}

bool isUnaryOperation(const std::vector<Token>& tokens)
{
	return tokens.empty() || tokens.back().type == Token::OPERATION && tokens.back().operation != ')';
}

std::vector<Token> getTokens(const std::string& expression)
{
	static const std::string operations = "+-/*^()";

	std::vector<Token> tokens;
	bool nextNegative = false;
	double number = 0;

	for (int32_t i = 0; i < expression.size(); i++)
	{
		if (expression[i] == '.')
		{
			if (isNoneDigitsAround(expression, i))
			{
				std::stringstream stream;
				stream << "Incorrect syntax at position " << i + 1 << std::endl;
				tokenizerErrorPrint(stream.str(), expression, i);
				return {};
			}

			number += getFractionalPart(expression, i + 1, i);
			tokens.push_back(Token(nextNegative ? -number : number));
			nextNegative = false;
			continue;
		}

		if (std::isdigit(static_cast<uint8_t>(expression[i])))
		{
			number = getIntegerPart(expression, i, i);
			if (i + 1 >= expression.size() || expression[i + 1] != '.')
			{
				tokens.push_back(Token(nextNegative ? -number : number));
				nextNegative = false;
			}
			continue;
		}

		if (expression[i] == ' ') continue;

		if (operations.find(expression[i]) != std::string::npos)
		{
			if (i + 1 < expression.size() && isDigitOrDot(expression, i + 1) && expression[i] == '+'
				&& isUnaryOperation(tokens))
				continue;

			if (i + 1 < expression.size() && isDigitOrDot(expression, i + 1) && expression[i] == '-'
				&& isUnaryOperation(tokens))
			{
				nextNegative = true;
				continue;
			}

			tokens.push_back(Token(expression[i]));
			continue;
		}

		std::stringstream stream;
		stream << "Unknown symbol \"" << static_cast<unsigned char>(expression[i])
			<< "\" at position " << i + 1 << std::endl;
		tokenizerErrorPrint(stream.str(), expression, i);
		return {};
	}

	return tokens;
}