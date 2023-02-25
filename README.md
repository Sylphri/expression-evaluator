# expression-evaluator
Small learning project, that permiss you evaluate simple mathematical expressions.

## **Syntax rules**:

* Numbers

Fractional part must be separated with dot ( . ) not comma ( , ).
If number integer part equals 0 it can be skipped.  
> Next two numbers are equal: 0.15 = .15

Unary plus and minus must be placed in front of number without spaces, like this: -3

* Operations

Evaluator support five operations: addition(+), subtraction(-), division(/), multiply(*), power(^) with correspond priorities. 
> Priority of operations can be changed with brackets.

Unary plus and minus before brackets can placed with spaces, like this: - (12 + 3). Its will not an error.

## **Usage in code**

You can use function evaluate() defined in header Evaluator.hpp. It takes expression string and return instance of struct EvaluateResult 
where field *success* indicates was expression successfuly evaluated or not. If it was, result stored in field *value*.

Or if you want somehow change evaluation process, you must follow next pattern. 
* First you have to split string into tokens with function getTokens(). 
* Next you need call function simplifyTokens() with tokens youve got. 
* After that call function verifyTokens() to verify correctness of expression. 
* And finally if verifyTokens() return *true* call function evaluateTokens() that return result of evaluation.
