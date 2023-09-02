# Builtin Functions

## IO Functions

### load

'load' function executes the given source file. It searches the default library path, which is normally '/usr/local/lib/tlisp' unless an installation prefix different from '/usr/local' is given. Global variables created by the included file are available after running the load function.

Example:
```
(load "util.tl")
(load "automata.tl")
```

'load' takes a single string argument. The return value is always (void). An error is thrown if it fails to find the file. 

There is also 'include' keyword which uses the same syntax, but it includes the contents of a file at parsing time instead of runtime so that macro definitions from the included file can be used. Unless you need to load a file whose name is determined at runtime, you should use include.

For both 'include' and 'load', the full path of the file must be given if it is not in the default library path. In the future it will also search local files, but this is currently not supported.

### display

'display' is used to write string representations of objects into the standard output. It is variadic and takes at least one argument. The arguments can be of any type. 

Example:

```
(display "some string" 123)
(display "a" "b")
```

Output is:

```console
some string123ab
```

In the output, there is no space between the arguments, and a newline is not automatically appended at end. If a newline is needed, the library function 'println' can be used instead.

### getchar

'getchar' is used to read a single character from the standard input. The returned value is an integer. It returns (void) when end of file occurs. An error is thrown in case of an IO error.

REPL example
```console
(getchar)
a
97
```

### putchar

putchar takes integers as arguments and writes them to the standard output. Each integer represents a single character. The return value is (void). An error is thrown in case of an IO error. 

REPL example
```console
(putchar 97 0x20 98 0x20 99 0x20 100)
a b c d
```

### current-seconds

current-seconds returns the Unix timestamp (in seconds).

REPL example
```console
(current-seconds)
1650553525
```

## Comparison Operators

In Theory Lisp, the only kind of equality is whether two objects contain equivalent data, and it is tested using = predicate which can take any type as argument.

= predicate can take one or more arguments. When it takes a single argument, the result is vacuously true. If it takes more than one argument, it is true if and only if all arguments are equal to each other.

!= predicate is equivalent to first applying the = predicate and then negating the result. When it takes a single argument, the result is false. If it takes more than one argument, it is true if and only if at least one argument is different from others.

**Equality of Numeric Types**

In Theory Lisp there are 3 numeric types: integer, real and rational. In order for two numbers to be equal, they must be numerically the same. Exact type equality is not required. For example,

```
(= 2 2.0)
(= 2 (/ 4 2))
(= 2.0 2)
(= 2.0 (/ 4 2))
(= (/ 4 2) 2)
(= (/ 4 2) 2.0)
```

all yield #t even though they are stored as different types. Note that not all numbers are exactly representable in floating point, so testing equality of real numbers is usually prone to errors. For example,

```
(= 9007199254740993.0 9007199254740992)
```

yields #t even though they are clearly different numbers. The reason for this result is that the first number 9007199254740993.0 is not exactly representable and becomes 9007199254740992.0 in the floating point representation.

**Equality of Booleans**

Two boolean values are equal if they have the same truth value.

```
(= #f #f) ; yields #t
(= #f #t) ; yields #f
(= #t #f) ; yields #f
(= #t #t) ; yields #t
```

**Equality of null**

Formally, there is only a single null instance, so all variables with null values are equal.

**Equality of void**

Formally, void denotes the lack of a return value. It should not be viewed as a value. For example, displaying the result of a define expression gives void because it does not a yield a value.

```console
(display (define x 3))
(void)
```

However, it is still possible to use void as a value because of the way interpreter is designed. A void 'value' can be generated using the function 'void', and two void values always compare equal. The ability to use void as a value may get removed in the future.

```console
(= (void) (void)) ; yields #t
```

**Equality of strings**

Two strings are equal if they have the same length, and contain the same characters in the same order.

```console
(= "abc" "abc") ; yields #t
(= "cba" "abc") ; yields #f
```

**Equality of procedures**

Comparing two procedures for equality always yields #f. There are no exceptions.

**Equality of pairs**

Two pairs are equal if and only if their first and second elements are equivalent, that is, (= A B) is true iff the following is true

```
(and (= (car A) (car B)) (= (cdr A) (cdr B)))
```

**Less than operator**

Less than operator is only defined for numeric types: integers, reals and rationals. Using any other type as argument to the less than operator will throw an "unsupported operation" error. The precision problems of numerical equality are also relevant to the less than operator. Other comparison operators (<=, >, >=) are defined in terms of the equality operator and the less than operator. <, <=, > and >= take exactly 2 arguments, and are not variadic.

## Type Predicates

For each builtin type, there is a type predicate that tests whether a given value has the corresponding type. They all take a single argument and return #t or #f.

```
null?
void?
boolean?
integer?
real?
rational?
number?
string?
pair?
procedure?
```

The only ones that may be confusing are real?, rational? and integer?. These predicates check how the given values are stored in the memory. If a value is stored as a floating point, real? yields true. If a value is stored as a rational number, rational? yields true. Similarly, if a number is stored as a machine integer, integer? yields true. They do not check whether the values are real numbers, rational numbers and integers in the mathematical sense. For example, (integer? 1.0) and (rational? 2) are both false, even though they are mathematically true.

## Arithmetic Operators

'+' is a variadic function that takes 0 or more arguments.

```
; If no arguments are given, result is 0.
(+) ; yields 0
; If a single argument is given, result is equal to that argument.
(+ 42) ; yields 42
; If 2 or more arguments are given, result is the sum of arguments.
(+ 1 2 3 4) ; yields 10
```

'*' is a variadic function that takes 0 or more arguments.

```
; If no arguments are given, result is 1.
(*) ; yields 1
; If a single argument is given, result is equal to that argument.
(* 42) ; yields 42
; If 2 or more arguments are given, result is the product of arguments.
(* 1 2 3 4) ; yields 24
```

'-' is a variadic function that takes 1 or more arguments.

```
; If a single argument is given, result is the additive inverse.
(- 42) ; yields -42
; If 2 or more arguments are given, all arguments after 
; the first argument are subtracted from the first argument
(- 10 1 2 3) ; yields 4
```

'/' is a variadic function that takes 1 or more arguments.

```
; If a single argument is given, result is the multiplicative inverse.
(/ 42) ; yields 1/42
; If 2 or more arguments are given, the first argument is divided
; by all of the remaining arguments
(/ 24 1 2 3) ; yields 4
```

## Boolean Operators

There are two versions of boolean conjunction and disjunction operators. 

For AND operator, 'and' (a macro) is short-circuiting, and '&' (builtin function) is not short-circuiting. 

Similarly, for OR operator, 'or' (a macro) is short-circuiting and '|' (builtin function is not short circuiting) 

Both versions are provided because when the evaluation of arguments is trivial, & and | can be faster, but in case each argument takes a long time to evaluate, 'and' and 'or' are better.

'xor' is the builtin function for boolean XOR operation, and 'not' is the builtin functin for boolean NOT operation.

## List Functions

'cons' creates a pair of values.

```
(cons 1 "a")
```

'list' creates a linked list of values.

```
(list 1 2 3)
; yields (cons 1 (cons 2 (cons 3 null)))
```

'car' returns the first element of a pair, and 'cdr' returns the second element of a pair

```
(car (cons 1 "a"))
; yields 1
(cdr (cons 1 "a"))
; yields "a"
```

 ## String Functions

Unlike most Lisp dialects, Theory Lisp source code is based on strings, not lists. All expressions and objects can be exactly represented as strings, and conversions between all types of objects and strings is possible. The following string functions frequently are needed, especially in macros.

### strlen

Returns the length of a string object. 

```
(strlen "string value")
; yields 12
```

### strcat

Concatenates given strings. The function takes zero or more arguments, and they can be of any type. Objects with types other than string are converted to their string representation before getting concatenated. The string representation of an object is guaranteed to yield the same object when used as argument to 'eval' function, including procedure objects. This is critical for macros to work.

Calling strcat without arguments yields empty string

```
(strcat) ; yields ""
```

Calling strcat with a single argument yields the string representation of the argument. The string representation does not have to be exactly equal to the argument as long as using it in 'eval' produces the same object. For an argument that is already a string the result is always the same.

```
> (strcat (lambda (...) (+ (car va_args) (* %(va_args)))))
"(lambda\0 (...) (+ (car va_args) (* %(va_args))))"
```

When strcat is called with more than one argument, it concatenates string representations of all arguments.

```
(strcat 42 "42" 42)
; yields "424242"
```

### charat

Returns the character at the given index. Usage: (charat str index)

```
(charat "string value" 4) ; yields "n"
```

There is no character type, so the result will be a string with a single character.

### substr

Returns substring of a string. Usage: (substr str begin end)
'begin' index is included, 'end' is excluded.

```
(substr "string value" 2 5) ; yields "rin"
```

### strcar

Returns the first character of the given string. 

```
(substr "string value") ; yields "s"
```

### strcdr

Returns all characters except the first character.

```
(substr "string value") ; yields "tring value"
```

Names are choosen to be similar to 'car' and 'cdr'.

### Special Characters

'newline', 'tab', 'backspace', 'quotation-mark' are functions with zero arguments. They yield "\n", "\t", "\b", "\"" respectively. Escape characters are not allowed inside string literals. They may get included in the future.

### i2s and s2i

In case ASCII characters different from the previous ones are required, 'i2s' and 's2i' can be used to make conversions between integers and characters.

i2s takes zero or more integer arguments and converts them to a string. Values outside of the ASCII range are not allowed. Also, 0 cannot be used.

```
> (i2s 0x6c 0x69 0x73 0x70)
"lisp"
```

s2i takes a string and returns a list of integers.

```
> (s2i "lisp")
(cons 108 (cons 105 (cons 115 (cons 112 null))))
```

## Reflection

### eval

'eval' is used to run code generated at runtime. It takes a single string argument and returns the value obtained from evaluating the expression contained in the string. All language features including macros are available inside 'eval'.

```
(eval "(+ 2 (* 3 4))")
; yields 14
```

### defined?

Returns true if the given variable has been defined either locally or in any outer scope.

```
(define x 42)
(defined? "x") ; yields #t
(defined? "y") ; yields #f
```

## Exceptions

### exit

Immediately stops the program. The interpreter does not enter into REPL when 'exit' is used. It is internally implemented with a specific type of error object, so it can be caught with a try-catch expression. This behaviour may get removed in the future.

```
(exit)
```

### error

Throws an error with the given error message. Errors can be caught using a try-catch expression. If an error is not handled, it propagates to the outermost scope and terminates the program with the given message. However, the interpeter still enters REPL after an error causes program to terminate, which can be used for debugging.

```
(error "error message")
```

## Math Functions

The math functions are wrappers for C standard library functions. Available functions are cos, sin, tan, acos, asin, atan, atan2, cosh, sinh, tanh, acosh, asinh, atanh, exp, log, log10, pow, sqrt, cbrt, hypot, erf, erfc, tgamma, lgamma, ceil, floor, trunc, round, modulo, isfinite, isinf, inan, and isnormal. Their behaviour is unchanged.

The only math function that behaves differently is 'random', which generates a uniformly distributed real number in the interval [0, 1)

