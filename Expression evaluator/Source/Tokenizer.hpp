#pragma once

#include <cstdint>

#include <string>
#include <vector>

#include "Token.hpp"

int32_t getIntegerPart(const std::string& expression, int32_t begin, int32_t& outEnd);
double getFractionalPart(const std::string& expression, int32_t begin, int32_t& outEnd);

bool isNoneDigitsAround(const std::string& expression, int32_t position);
bool isDigitOrDot(const std::string& expression, int32_t position);
bool isUnaryOperation(const std::vector<Token>& tokens);

std::vector<Token> getTokens(const std::string& expression);