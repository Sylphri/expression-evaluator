#pragma once

#include "Token.hpp"

#include <string>
#include <vector>

struct EvaluateResult {
    double value;
    bool success;
};

void evaluateOperation(std::vector<double> &numbers, char operation);

double evaluateTokens(const std::vector<Token> &tokens);

EvaluateResult evaluate(const std::string &expression);
