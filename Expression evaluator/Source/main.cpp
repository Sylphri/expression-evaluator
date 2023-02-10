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
		double number;
		char operation;
	};

	enum TokenType
	{
		NUMBER,
		OPERATION
	};

	TokenType type;

	Token(double number, TokenType type = Token::NUMBER)
		: number(number), type(type)
	{
	}

	Token(char operation, TokenType type = Token::OPERATION)
		: operation(operation), type(type)
	{
	}
};

void tokenizerErrorPrint(const std::string& message, const std::string& expression, int32_t errorPosition)
{
	std::string line(expression.size(), '-');
	line[errorPosition] = '^';
	std::cout << expression << std::endl << line << std::endl;
	std::cout << "[Tokenizer Error]: " << message;
}

std::vector<Token> getTokens(const std::string& expression)
{
	static const std::string operations = "+-/*^()";

	std::vector<Token> tokens;
	bool lastDigit = false;
	bool nextNegative = false;
	bool fractionalPart = false;
	double number = 0;
	double koefficient = 0.1;

	for (int32_t i = 0; i < expression.size(); i++)
	{
		if (expression[i] == '.')
		{
			if(fractionalPart || (i - 1 < 0 || !std::isdigit(expression[i - 1]))
				&& (i + 1 >= expression.size() || !std::isdigit(expression[i + 1])))
			{
				std::stringstream stream;
				stream << "Incorrect syntax at position " << i + 1 << std::endl;
				tokenizerErrorPrint(stream.str(), expression, i);
				return {};
			}

			fractionalPart = true;
			continue;
		}

		if (std::isdigit(static_cast<unsigned char>(expression[i])))
		{
			if (fractionalPart)
			{
				number += (expression[i] - '0') * koefficient;
				koefficient /= 10;
			}
			else
			{
				number *= 10;
				number += expression[i] - '0';
			}
			lastDigit = true;
			continue;
		}
		if (lastDigit)
		{
			if (nextNegative)
			{
				number *= -1;
				nextNegative = false;
			}

			tokens.push_back(Token(number));
			number = 0;
			koefficient = 0.1;
			lastDigit = false;
			fractionalPart = false;
		}

		if (expression[i] == ' ') continue;

		if (operations.find(expression[i]) != std::string::npos)
		{
			if (i < expression.size() - 1 && std::isdigit(expression[i + 1]) && expression[i] == '+'
				&& (tokens.empty() || tokens.back().type == Token::OPERATION && tokens.back().operation != ')'))
				continue;

			if (i < expression.size() - 1 && (std::isdigit(expression[i + 1]) || expression[i + 1] == '.') && expression[i] == '-'
				&& (tokens.empty() || tokens.back().type == Token::OPERATION && tokens.back().operation != ')'))
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

	if (lastDigit)
	{
		if (nextNegative)
			number *= -1;
		tokens.push_back(Token(number));
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
			tokens[i] = Token('(');
			tokens.insert(tokens.begin() + i + 1, { Token(-1.0), Token('*') });
			int32_t brackets = 1;
			int32_t j = i + 4;
			for (; j < tokens.size() && brackets > 0; ++j)
			{
				if (isOperation(tokens[j], '('))
					++brackets;
				else if (isOperation(tokens[j], ')'))
					--brackets;
			}
			tokens.insert(tokens.begin() + j, Token(')'));
		}
	}
}

bool verifyTokens(const std::vector<Token>& tokens)
{
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
	case '^':
		numbers.push_back(pow(first, second));
		break;
	}
}

double evaluateTokens(const std::vector<Token>& tokens)
{
	static std::unordered_map<char, int8_t> operationPriorities = {
		{'+', 1}, {'-', 1},
		{'*', 2}, {'/', 2},
		{'^', 3}
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
	std::cout << "Expression evaluator v1.3. Support integers, real numbers, five operations(+, -, *, /, ^) and brakets.\n";
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