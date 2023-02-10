#include <cstdint>

#include <string>
#include <vector>
#include <unordered_map>

#include "Token.hpp"
#include "Tokenizer.hpp"
#include "Simplifyer.hpp"
#include "Verifyer.hpp"
#include "Evaluator.hpp"

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

EvaluateResult evaluate(const std::string& expression)
{
	std::vector<Token> tokens = getTokens(expression);
	if (tokens.empty()) return { 0.0, false };
	simplifyTokens(tokens);
	if (verifyTokens(tokens))
		return { evaluateTokens(tokens), true };
	return { 0.0, false };
}