#include <string>
#include <iostream>

#include "Evaluator.hpp"

int main()
{
	std::cout << "Expression evaluator v1.3. Support integers, real numbers, five operations(+, -, *, /, ^) and brackets.\n";
	while (true)
	{
		std::cout << "Enter expression: ";
		std::string expression;
		std::getline(std::cin, expression);
		EvaluateResult result = evaluate(expression);
		if (result.success)
			std::cout << "result: " << result.value << std::endl;
	}
}