#include <vector>

#include "Token.hpp"
#include "Utilities.hpp"

bool isNoneBeforeOrOperation(const std::vector<Token>& tokens, int32_t position)
{
	return position - 1 < 0 || tokens[position - 1].type == Token::OPERATION;
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