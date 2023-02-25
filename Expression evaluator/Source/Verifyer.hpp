#pragma once

#include "Token.hpp"

#include <vector>

bool isThatNotBracket(Token token);
bool hasTwoNeighbourTokens(const std::vector<Token> &tokens, int32_t position);

bool verifyTokens(const std::vector<Token> &tokens);
