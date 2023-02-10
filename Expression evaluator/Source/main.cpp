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

bool isOperation(Token token, char operation)
{
	return token.type == Token::OPERATION && token.operation == operation;
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
			if(isNoneDigitsAround(expression, i))
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

bool isNoneBeforeOrOperation(const std::vector<Token>& tokens, int32_t position)
{
	return position - 1 < 0 || tokens[position - 1].type == Token::OPERATION;
}

bool isNextThatOperation(const std::vector<Token>& tokens, int32_t position, char operation)
{
	return position + 1 < tokens.size() && isOperation(tokens[position + 1], operation);
}

void simplifyTokens(std::vector<Token>& tokens)
{
	for (int32_t i = 0; i < tokens.size() - 1; ++i)
	{
		if (isOperation(tokens[i], '+') && isNextThatOperation(tokens, i, '(') && isNoneBeforeOrOperation(tokens, i))
		{
			tokens.erase(tokens.begin() + i, tokens.begin() + i + 1);
		}

		if (isOperation(tokens[i], '-') && isNextThatOperation(tokens, i, '(') && isNoneBeforeOrOperation(tokens, i))
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

bool isThatNotBracket(Token token)
{
	return token.type == Token::OPERATION && (token.operation != '(' && token.operation != ')');
}

bool hasNotTwoNeighbourTokens(const std::vector<Token>& tokens, int32_t position)
{
	return position - 1 < 0 && position + 1 == tokens.size();
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

		if(tokens[i].type == Token::OPERATION && !isOperation(tokens[i], '(') && !isOperation(tokens[i], ')')
			&& isNextThatOperation(tokens, i, '('))
		{
			i++;
			brackets++;
			continue;
		}

		if (i + 1 < tokens.size() && tokens[i].type == Token::NUMBER && tokens[i + 1].type == Token::NUMBER)
		{
			std::stringstream stream;
			stream << "Two number in a row[" << tokens[i].number << ", " << tokens[i + 1].number << "]";
			verifyerErrorPrint(tokens, i, stream.str());
			return false;
		}

		if (isOperation(tokens[i], ')') && i + 1 < tokens.size() && tokens[i + 1].type == Token::NUMBER)
		{
			std::stringstream stream;
			stream << "Missing operation [" << tokens[i].operation << ", " << tokens[i + 1].number << "]";
			verifyerErrorPrint(tokens, i, stream.str());
			return false;
		}

		if (tokens[i].type == Token::NUMBER && isNextThatOperation(tokens, i, '('))
		{
			std::stringstream stream;
			stream << "Missing operation [" << tokens[i].number << ", " << tokens[i + 1].operation << "]";
			verifyerErrorPrint(tokens, i, stream.str());
			return false;
		}

		if (isThatNotBracket(tokens[i]) && (hasNotTwoNeighbourTokens(tokens, i)
			|| tokens[i - 1].type == Token::OPERATION && tokens[i - 1].operation != ')'
			|| tokens[i + 1].type == Token::OPERATION && tokens[i + 1].operation != '('))
		{
			std::stringstream stream;
			stream << "Missing operation argument [" << tokens[i].operation << "]";
			verifyerErrorPrint(tokens, i, stream.str());
			return false;
		}

		if (isOperation(tokens[i], '(') && i + 1 < tokens.size() && isOperation(tokens[i + 1], ')'))
		{
			std::stringstream stream;
			stream << "Empty brackets";
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
	std::cout << "Expression evaluator v1.3. Support integers, real numbers, five operations(+, -, *, /, ^) and brackets.\n";
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