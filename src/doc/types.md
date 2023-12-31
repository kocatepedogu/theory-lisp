# Types

Theory Lisp is a dynamically and strongly typed language. There are 9 builtin types.

### Error

Error is a special type in Theory Lisp. If the evaluation of an expression yields an error, the receiver immediately stops its normal execution and returns the error it received unchanged.

Whenever an error is generated, it propagates to the outermost scope and terminates the program with an error message. A user defined error can be thrown by the builtin function "error"

In order make the program continue execution, a [try-catch expression](expressions.md#Try-Catch-Expression) can be used to handle the error.

```
(define func
  (lambda (x)
    (if (< x 0)
      (error "x cannot be negative")
        (+ x 1))))
```

### Boolean

Theory Lisp provides a boolean type which can be either true or false. Conditional expressions only accept boolean values. In most other Lisp dialects, nil is interpreted as false, and anything else is true, but this is not the case in Theory Lisp. Trying to use a type other than boolean in a conditional expression will cause a runtime error.

#t and #f literals are used to generate true and false boolean values.

### Integer

Integer type stores signed C integers, whose range is implementation-defined.

### Real

Real type stores C doubles, whose range and precision is implementation-defined.

### Null

Null denotes empty list. A null value is generated by the literal null.

### Pair

Pair type is a tuple that contains two objects. A pair is generated by the builtin function "cons". 

```
(cons "abc" 123)
```

A list is a chain of pairs. For example, the list generated by 

```
(list 1 2 3 4)
```

is equivalent to the result of

```
(cons 1 (cons 2 (cons 3 (cons 4 null))))
```

The notations '(1 2 3) and '(1 . 2) are not available for lists and pairs. Also, there is no 'quote' expression.

### Procedure

Functions are first class objects in Theory Lisp. A procedure object stores both the executable code and the closure, which contains the variables captured from the original environment.

Lambdas, Polish notation expressions, and automatons yield procedure objects.

```
(lambda (x) (+ x 1)
{+ $1 * $2 $3}
```

### String

Strings store ASCII character sequences, and a string is allowed contain any character other than \0. 

A string literal is used generate strings.
```
"string value"
```

Conversion between lists of ASCII integers and strings can be done by the builtin functions s2i and i2s.

### Void

Void type denotes the lack of a return value. An "instance" of void can be generated by calling the builtin function void.