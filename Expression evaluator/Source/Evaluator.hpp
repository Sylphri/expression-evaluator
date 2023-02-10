#pragma once

#include <vector>
#include <string>

#include "Token.hpp"

struct EvaluateResult
{
	double value;
	bool succes;
};

void evaluateOperation(std::vector<double>& numbers, char operation);

double evaluateTokens(const std::vector<Token>& tokens);

EvaluateResult evaluate(const std::string& expression);