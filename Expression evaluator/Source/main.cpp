#include <cstdint>

#include <vector>
#include <unordered_map>
#include <string>

#include <iostream>
#include <sstream>

struct Token
{
	union
	{
		int32_t number;
		char operation;
	};

	enum TokenType
	{
		NUMBER,
		OPERATION
	};

	TokenType type;
};

std::vector<Token> getTokens(const std::string& expression)
{
	static const std::string operations = "+-/*()";

	std::vector<Token> tokens;
	bool lastDigit = false;
	bool nextNegative = false;
	int32_t number = 0;

	for (size_t i = 0; i < expression.size(); i++)
	{
		if (std::isdigit(static_cast<unsigned char>(expression[i])))
		{
			number *= 10;
			number += expression[i] - '0';
			lastDigit = true;
			continue;
		}
		else if (lastDigit)
		{
			if (nextNegative)
			{
				number *= -1;
				nextNegative = false;
			}

			tokens.push_back({ number, Token::NUMBER });
			number = 0;
			lastDigit = false;
		}

		if (expression[i] == ' ') continue;

		if (operations.find(expression[i]) != std::string::npos)
		{
			if (i < expression.size() - 1 && std::isdigit(expression[i + 1]) && expression[i] == '-'
				&& (tokens.empty() || tokens.back().type == Token::OPERATION) && tokens.back().operation != ')')
			{
				nextNegative = true;
				continue;
			}

			tokens.push_back({ expression[i], Token::OPERATION });
			continue;
		}

		std::cout << "[Tokenizer Error]: Unknown symbol \"" << static_cast<unsigned char>(expression[i]) 
			<< "\" at position " << i + 1 << std::endl;
		std::string line(expression.size(), '-');
		line[i] = '^';
		std::cout << expression << std::endl << line << std::endl;
		return {};
	}

	if (lastDigit)
	{
		if (nextNegative)
		{
			number *= -1;
			nextNegative = false;
		}
		tokens.push_back({ number, Token::NUMBER });
	}

	return tokens;
}

std::string concatTokens(const std::vector<Token> tokens, int32_t highlightToken, int32_t& outPosition)
{
	std::string expression;
	int32_t position = 0;
	for (int32_t i = 0; i < tokens.size(); i++)
	{
		if (i <= highlightToken)
			position = expression.size();
		if (tokens[i].type == Token::OPERATION && tokens[i].operation == ')')
			expression.pop_back();
		int32_t size = expression.size();
		expression += (tokens[i].type == Token::NUMBER ? std::to_string(tokens[i].number)
			: std::string(1, tokens[i].operation)) + " ";
		if (tokens[i].type == Token::OPERATION && tokens[i].operation == '(')
			expression.pop_back();
	}
	outPosition = position;
	return expression;
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

bool isOperation(Token token, char operation)
{
	return token.type == Token::OPERATION && token.operation == operation;
}

void simplifyTokens(std::vector<Token>& tokens)
{
	for (int32_t i = 0; i < tokens.size() - 1; ++i)
	{
		if (i + 1 < tokens.size() && isOperation(tokens[i], '+') && isOperation(tokens[i + 1], '(')
			&& (i - 1 < 0 || tokens[i - 1].type != Token::NUMBER))
		{
			tokens.erase(tokens.begin() + i, tokens.begin() + i + 1);
		}

		if (i + 1 < tokens.size() && isOperation(tokens[i], '-') && isOperation(tokens[i + 1], '(')
			&& (i - 1 < 0 || tokens[i - 1].type != Token::NUMBER))
		{
			tokens.erase(tokens.begin() + i, tokens.begin() + i + 1);
			int32_t brackets = 1;
			for (int32_t j = i + 1; j < tokens.size() && brackets > 0; ++j)
			{
				if (isOperation(tokens[j], ')'))
				{
					--brackets;
					continue;
				}

				if (tokens[j].type == Token::NUMBER)
					tokens[j].number *= -1;
			}
		}
	}
}

bool verifyTokens(const std::vector<Token>& tokens)
{
	if (tokens.size() < 3)
	{
		std::cout << "[Verifyer Error]: Invalid expression\n";
		return false;
	}

	int32_t brackets = 0;
	for (int32_t i = 0; i < tokens.size(); i++)
	{
		if (tokens[i].type == Token::OPERATION)
		{
			if (tokens[i].operation == '(')
				brackets++;
			else if (tokens[i].operation == ')')
				brackets--;
		}

		if (i + 1 < tokens.size() && tokens[i].type == Token::OPERATION && tokens[i + 1].type == Token::OPERATION
			&& tokens[i].operation != '(' && tokens[i].operation != ')' && tokens[i + 1].operation == '(')
		{
			i++;
			brackets++;
			continue;
		}

		if (i + 1 < tokens.size() && tokens[i].type == Token::NUMBER && tokens[i + 1].type == Token::NUMBER)
		{
			std::stringstream stream;
			stream << "Two number in a row[" << tokens[i].number
				<< ", " << tokens[i + 1].number << "]";
			verifyerErrorPrint(tokens, i, stream.str());
			return false;
		}

		if (i + 1 < tokens.size() && tokens[i].type == Token::OPERATION && tokens[i + 1].type == Token::NUMBER
			&& tokens[i].operation == ')')
		{
			std::stringstream stream;
			stream << "Missing operation [" << tokens[i].operation
				<< ", " << tokens[i + 1].number << "]";
			verifyerErrorPrint(tokens, i, stream.str());
			return false;
		}

		if (i + 1 < tokens.size() && tokens[i].type == Token::NUMBER && tokens[i + 1].type == Token::OPERATION
			&& tokens[i + 1].operation == '(')
		{
			std::stringstream stream;
			stream << "Missing operation [" << tokens[i].number
				<< ", " << tokens[i + 1].operation << "]";
			verifyerErrorPrint(tokens, i, stream.str());
			return false;
		}

		if (tokens[i].type == Token::OPERATION && tokens[i].operation != '(' && tokens[i].operation != ')'
			&& ((i - 1 < 0 || i + 1 == tokens.size())
			|| tokens[i - 1].type != Token::NUMBER && tokens[i - 1].operation != ')' 
			|| tokens[i + 1].type != Token::NUMBER && tokens[i + 1].operation != '('))
		{
			std::stringstream stream;
			stream << "Missing operation argument [" << tokens[i].operation << "]";
			verifyerErrorPrint(tokens, i, stream.str());
			return false;
		}
	}

	if (brackets > 0)
	{
		std::cout << "[Verifyer Error]: Missing " << brackets << " closing brackets\n";
		return false;
	}
	if (brackets < 0)
	{
		std::cout << "[Verifyer Error]: Missing " << -brackets << " opening brackets\n";
		return false;
	}
	return true;
}

void evaluateOperation(std::vector<double>& numbers, char operation)
{
	double second = numbers.back();
	numbers.pop_back();
	double first = numbers.back();
	numbers.pop_back();

	switch (operation)
	{
	case '+':
		numbers.push_back(first + second);
		break;
	case '-':
		numbers.push_back(first - second);
		break;
	case '*':
		numbers.push_back(first * second);
		break;
	case '/':
		numbers.push_back(first / second);
		break;
	}
}

double evaluateTokens(const std::vector<Token>& tokens)
{
	static std::unordered_map<char, int8_t> operationPriorities = {
		{'+', 1}, {'-', 1},
		{'*', 2}, {'/', 2}
	};

	std::vector<double> numbers;
	std::vector<char> operations;
	for (const Token& token : tokens)
	{
		if (token.type == Token::NUMBER)
		{
			numbers.push_back(token.number);
			continue;
		}

		if (token.operation == '(')
		{
			operations.push_back('(');
			continue;
		}

		if (token.operation == ')')
		{
			while (operations.back() != '(')
			{
				evaluateOperation(numbers, operations.back());
				operations.pop_back();
			}
			operations.pop_back();
			continue;
		}

		if (operations.empty() || operations.back() == '('
			|| operationPriorities[token.operation] > operationPriorities[operations.back()])
		{
			operations.push_back(token.operation);
			continue;
		}

		while (!operations.empty() && operations.back() != '('
			&& operationPriorities[token.operation] <= operationPriorities[operations.back()])
		{
			evaluateOperation(numbers, operations.back());
			operations.pop_back();
		}
		operations.push_back(token.operation);
	}

	while (!operations.empty())
	{
		evaluateOperation(numbers, operations.back());
		operations.pop_back();
	}

	return numbers.size() > 0 ? numbers[0] : 0;
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

int main()
{
	std::cout << "Expression evaluator v1.1. Support integers, four operations(+, -, *, /) and brakets.\n";
	while (true)
	{
		std::cout << "Enter expression: ";
		std::string expression;
		std::getline(std::cin, expression);
		std::vector<Token> tokens = getTokens(expression);
		if (tokens.size() == 0) continue;
		simplifyTokens(tokens);
		if (verifyTokens(tokens))
			std::cout << "result: " << evaluateTokens(tokens) << "\n\n";
	}
}