#include "Token.hpp"
#include "Utilities.hpp"

#include <iostream>
#include <sstream>
#include <vector>

bool isThatNotBracket(Token token) {
    return token.type == Token::OPERATION
           && (token.operation != '(' && token.operation != ')');
}

bool hasTwoNeighbourTokens(const std::vector<Token> &tokens, int32_t position) {
    return position - 1 >= 0 && position + 1 < tokens.size();
}

bool verifyTokens(const std::vector<Token> &tokens) {
    int32_t brackets = 0;
    for (int32_t i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == Token::OPERATION) {
            if (tokens[i].operation == '(')
                brackets++;
            else if (tokens[i].operation == ')')
                brackets--;
        }

        if (tokens[i].type == Token::OPERATION && !isOperation(tokens[i], '(')
            && !isOperation(tokens[i], ')') && isNextOperation(tokens, i, '(')) {
            i++;
            brackets++;
            continue;
        }

        if (i + 1 < tokens.size() && tokens[i].type == Token::NUMBER
            && tokens[i + 1].type == Token::NUMBER) {
            std::stringstream stream;
            stream << "Two number in a row[" << tokens[i].number << ", "
                   << tokens[i + 1].number << "]";
            verifyerErrorPrint(tokens, i, stream.str());
            return false;
        }

        if (isOperation(tokens[i], ')') && i + 1 < tokens.size()
            && tokens[i + 1].type == Token::NUMBER) {
            std::stringstream stream;
            stream << "Missing operation [" << tokens[i].operation << ", "
                   << tokens[i + 1].number << "]";
            verifyerErrorPrint(tokens, i, stream.str());
            return false;
        }

        if (tokens[i].type == Token::NUMBER && isNextOperation(tokens, i, '(')) {
            std::stringstream stream;
            stream << "Missing operation [" << tokens[i].number << ", "
                   << tokens[i + 1].operation << "]";
            verifyerErrorPrint(tokens, i, stream.str());
            return false;
        }

        if (isThatNotBracket(tokens[i])
            && (!hasTwoNeighbourTokens(tokens, i)
                || tokens[i - 1].type == Token::OPERATION
                       && tokens[i - 1].operation != ')'
                || tokens[i + 1].type == Token::OPERATION
                       && tokens[i + 1].operation != '(')) {
            std::stringstream stream;
            stream << "Missing operation argument [" << tokens[i].operation << "]";
            verifyerErrorPrint(tokens, i, stream.str());
            return false;
        }

        if (isOperation(tokens[i], '(') && i + 1 < tokens.size()
            && isOperation(tokens[i + 1], ')')) {
            std::stringstream stream;
            stream << "Empty brackets";
            verifyerErrorPrint(tokens, i, stream.str());
            return false;
        }
    }

    if (brackets > 0) {
        std::cout << "[Syntax Error]: Missing " << brackets << " closing brackets\n";
        return false;
    }
    if (brackets < 0) {
        std::cout << "[Syntax Error]: Missing " << -brackets << " opening brackets\n";
        return false;
    }
    return true;
}
