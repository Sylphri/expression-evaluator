#pragma once

#include <vector>

#include "Token.hpp"

bool isNoneBeforeOrOperation(const std::vector<Token>& tokens, int32_t position);

void simplifyTokens(std::vector<Token>& tokens);