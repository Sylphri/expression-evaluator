#pragma once

#include "Token.hpp"

#include <string>
#include <vector>

std::string concatTokens(const std::vector<Token> tokens, int32_t highlightToken,
                         int32_t &outPosition);

void printTokens(const std::vector<Token> &tokens);

void tokenizerErrorPrint(const std::string &message, const std::string &expression,
                         int32_t errorPosition);

void verifyerErrorPrint(const std::vector<Token> &tokens, int32_t errorToken,
                        const std::string &message);

bool isOperation(Token token, char operation);
bool isNextOperation(const std::vector<Token> &tokens, int32_t position, char operation);
