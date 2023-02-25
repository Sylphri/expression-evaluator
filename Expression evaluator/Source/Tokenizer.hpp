#pragma once

#include "Token.hpp"

#include <string>
#include <vector>

int32_t integerPart(const std::string &expression, int32_t begin, int32_t &outEnd);
double fractionalPart(const std::string &expression, int32_t begin, int32_t &outEnd);

bool isNoneDigitsAround(const std::string &expression, int32_t position);
bool isDigitOrDot(const std::string &expression, int32_t position);
bool isUnaryOperation(const std::vector<Token> &tokens);

std::vector<Token> getTokens(const std::string &expression);
