# Expressions

## Function Evaluation Expression

A function evaluation consists of a function (first expression) and function arguments (remaining expressions). Function arguments are fully evaluated before the function is called. The evaluation order is always left to right.

'load' is a builtin function that executes another Theory Lisp source at runtime.

```
(load "file.tl")
```

'+' is a builtin arithmetic function

```
; yields 6
(+ 1 2 3)
```

'display' is a builtin function that prints its arguments.

```
; prints 1,2,3
(display 1 "," 2 "," 3)
```

'begin' is a builtin function that returns its last argument. It is used to implement sequencing in Theory Lisp.

```
; Prints 123
(begin
  (display 1)
  (display 2)
  (display 3)
)
```

**Expanding a list object in function evaluation**

Suppose that a function named 'func' takes 3 arguments, and the values that are going to be passed are currently stored in a list named 'lst'.

The obvious way to pass the list contents is the following:

```
(func (car lst) (cadr lst) (car (cddr lst)))
```

This is long and error-prone. Theory Lisp provides % operator to expand a list in a function evaluation, so that the same thing can be done in a much shorter way. Also, the number of formal parameters does not need to be known for using the function. The function may even be variadic, in which case % can be used for forwarding variadic arguments.

```
(func %lst)
```

## Define Expression

Define expression creates a variable in the global scope.

```
(define x 3)
; x is 3
(define y (+ 2 3))
; y is 5
```

define can be used to modify an existing global variable

```
(define x 3)
; x is initially 3
(define x (+ x 1))
; x is now 4
```

define can be used inside another expression. Define expressions always yield (void)

```
(display (define x 1))
; Prints (void)
(display x)
; Prints 1
```

define can be used to write imperative code as it is allowed to mutate existing variables.

## Let Expression

Let expression is used to declare local variables. A let expression consists of a list of variable declarations and a body.

The list of variable declarations can be empty
```
(let () (+ 2 3)) ; yields 5
```
The list of variable declarations can contain one or more variables
```
(let ((x 1) (y 2)) (+ x y)) ; yields 3
```
Variables with the same name in outer scope are not affected.
```
(define x 1)

; Prints 2
(let ((x 2)) 
   (display x))
   
; Prints 1
(display x)
```

## Set Expression

Set expression modifies an existing variable, or creates a new local variable if the variable with the given name does not exist.

Creates x in the local stack frame of let expression. Displayed value is 1.

```
(let ()
  (begin
    (set! x 1)
    (display x)))
```

Local x is initialized to 1. set! modifies the value of x to 2. Displayed value is 2.

```
(let ((x 1))
  (begin
    (set! x (+ x 1))
    (display x)))
```

Global x is initialized to 1. set! tries to find x in the local scope without success, so it continues to search in outer scopes. It finds global x and modifies its value to 2. Displayed value is 2

```
(define x 1)
(let ()
  (set! x (+ x 1))
(display x)
```

set! can be used to write imperative code as it is allowed to mutate existing variables.

## If Expression

If expression provides branching in Theory Lisp. The expression consists of a condition expression, an expression that gets evaluated when the condition is true, and an expression that gets evaluated when the condition is false.

```
(if #t 1 2) ; yields 1
(if #f 1 2) ; yields 2
```

```
(if #t (display 1) (display 2)) ; prints 1
(if #f (display 1) (display 2)) ; prints 2
```

Using a value other than boolean as condition causes a runtime error. This is different from most other Lisp variants.

```
(if null 1 2) ; throws a runtime error
```

## Cond Expression

Cond expression is used to implement multiway branching. The expression consists of a list of pairs. Each pair has a condition followed by an expression that gets evaluated when the condition is true.

```
(define x 0.5)

; yields 1
(cond
  ((< x 0) -1)
  ((= x 0) 0)
  ((> x 0) 1)
)
```

'else' is an alias for #t defined in "util.tl". It can be used as a default case in cond expression.

```
(include "util.tl")

(cond
  ((< x 0) -1)
  (else 1)
)
```

Cond expression yields (void) if none of the conditions is satisfied.

```
(define x 0)

; yields (void)
(cond
  ((< x 0) -1)
  ((> x 0) 1)
)
```

Similar to if expression, condition must evaluate to a boolean value. Using another type as condition causes a runtime error.

```
(cond
  (null 0) ; throws a runtime error
  (else 1)
)
```

## Lambda Expression

Lambda expression is used to create a function. It consists of
1. an optional prefix arity
2. an optional capture list
3. a formal parameter list and
4. a body.

**Lambdas with Fixed Number Of Parameters**

The following is a function with a single argument that computes the factorial of a given number. This is also valid code in Scheme.

```
(define factorial
  (lambda (n)
    (if (= 0 n)
      1
      (* n (factorial (- n 1))))))
```

The number of actual arguments must exactly match the number of formal parameters. Otherwise, a runtime error is thrown.

```
(define func
  (lambda (x y)
    (+ x y)))
    
(func 1 2 3) ; Runtime error
(func 1 2) ; OK
(func 1) ; Runtime error
```

The same factorial function can be written with an explicit prefix arity, but this is not required. Prefix arity is automatically set to the number of parameters when it is not given.

```
(define factorial
  (lambda\1 (n)
    (if (= 0 n)
      1
      (* n (factorial (- n 1))))))
```

If the given prefix arity does not match the number of formal parameters, a parser error is generated. This can be used in functions with too many parameters to ensure that the number of parameters is correct.

```
(define func
  (lambda\3 (x y) ; Parser error. Real arity is 2, not 3.
    (+ x y)))
```

**Lambdas with Variable Number of Parameters**

Variadic lambdas can take unlimited number of arguments. A variadic lambda is defined by using ellipsis '...' in the formal parameter list. The list of actual variadic arguments is stored in the automatically generated local variable named 'va_args'.

The following is a variadic function that returns the sum of arguments. It also shows the use of % for forwarding variadic arguments.
```
(define sum
  (lambda (...)
    (if (null? va_args)
      0
      (+ (car va_args) (sum %(cdr va_args)))
    )
  )
)

(sum 1 2 3 4) ; yields 10
```

It is possible for a lambda to have some fixed parameters before the ellipsis. For example, the following function finds the sum of variadic arguments and then multiplies the result with the given coefficient

```
(define func
  (lambda (coeff ...)
    (* coeff (+ %va_args))
  )
)

(func 2 1 2 3) ; yields 12
```

The number of actual arguments must be at least equal to the number of fixed parameters, and it can be larger. 

```
(define func
  (lambda (x y ...)
    (+ x y %va_args)
  )
)

(func 1) ; runtime error
(func 1 2) ; yields 3
(func 1 2 3) ; yields 6
```

The prefix arity of a variadic lambda is automatically set to the number of fixed parameters. If the function does not have any fixed parameters, the prefix arity is set to 0. This can be overriden by writing a backslash symbol and an arity value like below. The prefix arity is allowed to be any value greater than or equal to the number of fixed parameters.

```
(define multiply)
  (lambda\2 (...)
    (* %va_args)))
```

The prefix arity only matters if the lambda is used inside a Polish notation expression.

**Capture List**

Capture list contains the names of variables whose values will be captured from the environment where the lambda is created. Square brackets are used to write a capture list.

```
(define make-predicate
  (lambda (low high)
    (lambda [low high] (x)
      (and (> x low) (< x high)))))
```

The function make-predicate returns a predicate that takes a number x and returns true if it is in the specified range. For example,

```
(define pred
  (make-predicate 10 30))
```

pred is a function with a single argument that returns true if its argument is in the interval (10, 30).

```
(pred 20) ; yields #t
(pred 50) ; yields #f
```

The capture list [low high] in the first code block tells the interpreter that the values of low and high at the moment the procedure is created must be incorporated into the closure of the procedure. If the capture list were not used, (pred 20) would throw an error because it would not be able to find the variables low and high.

## Polish Notation Expression

Polish notation expression is another way to define functions, but it is usually much shorter than equivalent lambda expressions. For example,

```
(lambda (x y z) (+ x (* y z)))
```

can be written as

```
{+ $1 * $2 $3}
```

**Syntax**

PN expression body contains procedure objects and objects that are arguments to those procedures.

Functions defined using Polish notation are always variadic. Each argument can be accessed through automatically generated local variables $1, $2, $3, $4, ... The total number of arguments is stored in the variable 'nargs'.

```
(define f1 
  {display + $2 $3})
(define f2
  {display nargs})

(f1 1 2 3) ; Prints 5
(f2 1 2 3) ; Prints 3
```

**Implicit Insertions**

Each procedure inside the PN expression body expects as many arguments as its prefix arity. When some arguments are missing, the missing arguments are inserted from the left using the values of $1, $2, $3,... Examples are more clear than a formal description.

**Example 1**

The prefix arity of > is 2, so it expects two arguments like below.

```
{> $1 3}
```

The following is also valid and equivalent.

```
{> 3}
```

**Example 2**

The prefix arity of * is 2, so it expects two arguments. One argument is $1 which becomes 6, and the other is - 5 3 which yields 2. The entire expression yields 12.

```
({* $1 - 5 3} 6)
```

The following, where the first argument of * is missing, is also valid and equivalent.

```
({* - 5 3} 6)
```

**Example 3**

The prefix arity of - is 2, so it expects two arguments like below. The entire expression yields 4.

```
({- $1 $2} 7 3)
```

The following is also valid and equivalent. In this case both $1 and $2 are inserted.

```
({-} 7 3)
```

Unless the operation is trivial such as {= 2}, {> 3}, {< 0} it is not a good idea to write functions that depend on implicit insertion of arguments. It reduces readability and is prone to errors. When operations are trivial, however; using implicit insertions makes large programs more readable.

Warning: If there are multiple procedures with missing arguments in a PN expression, the behaviour is undefined. Use implicit insertions only if just a single procedure has missing arguments. Also, always ensure that the missing arguments are on the left, not on the right.

**PN expressions with a single value**

A PN expression with a single object in its body (not a procedure object) is equivalent to a lambda that returns that value.

```
{123}
```
is equivalent to the lambda
```
(lambda (...) 123)
```

This property can be used to embed an S-expression inside a PN expression.
```
{(if $1 (+ $2 $3) (- $2 $3))}
{(set! x 1)}
```

Similar to lambda expressions, a capture list can be added at the beginning of the PN expression

```
(define make-predicate
  (lambda (low high)
    {[low high] and > $1 low < $1 high}))
```

PN expressions reduce length of functions significantly, but there are also some drawbacks. Getting rid of parenthesis, and using implicit insertions require restrictions that does not exist in lambdas.

Every procedure in the body must take fixed number of parameters. To be able to use a variadic function in PN expressions, a reasonable prefix arity must be given in its definition. Then the procedure can be used as if it had a fixed number of parameters with that arity. Builtin arithmetic operators are variadic, and accept zero or more arguments, but they have a prefix arity of 2.

**PN expression inside PN expression**

Since procedures created using PN expressions are always variadic, using a PN procedure inside another PN expression requires a prefix arity.

When no prefix arity is given, the prefix arity of a PN expression is automatically set to 0. This can be overriden by writing the prefix arity after a backslash similar to lambda expressions. The prefix arity of the following PN expression is 2.

```
{\2 + $1 $2}
```

Now, it is possible to use this PN procedure inside another PN expression

```
{{\2 + $1 $2} 3 4}
```

**Using a procedure as argument to another procedure**

If a procedure object is an argument to another procedure in a PN expression, wrap it using the 'wrap' function defined in 'util.tl'. For example, in the following code,

```
(define increment (lambda (x) (+ x 1)))
(define list-increment {map increment $1})
(list-increment (list 1 2 3))
; throws error
```

it seems that 'increment' and '$1' are going to be passed as arguments to 'map'. What actually happens is that '$1' is passed to 'increment', which throws an error. The solution to this problem is to wrap 'increment'.

```
(define increment (lambda (x) (+ x 1)))
(define list-increment {map (wrap increment) $1})
(list-increment (list 1 2 3))
; yields a list with values 2 3 4
```

## Automaton Expression

An automaton in Theory Lisp is a machine that can be used to simulate deterministic Turing machines and finite state machines. Automatons take tapes as arguments and return final tape contents combined with a decision value. 

An automaton expression consists of 
1. An arity (number of tapes that the machine operates on)
2. An optional capture list
3. A list of states

An automaton expression begins with the keyword 'automaton' followed by an arity that denotes the number of tapes the machine operates on. The arity is mandatory. In the following automaton, '\1' means that machine uses a single tape.

```
(automaton\1
   STATES ARE WRITTEN HERE
)
```

Frequently, it is necessary to generate automatons based on some parameter at runtime. An optional capture list can be used.

```
(define make-automaton
  (lambda (some-parameter)
    (automaton\1 [some-parameter]
      STATES ARE WRITTEN HERE
    )
  )
)
```

Each state begins with a state name followed by a sequence of transitions. The following machine has two states named q0 and q1. The first state is always the start state.

```
(automaton\1
  (q0
    TRANSITIONS ARE WRITTEN HERE
  )
  (q1
    TRANSITIONS ARE WRITTEN HERE
  )
)
```

Each transition consists of a transition condition (a function), head operations for each tape, and an identifier that denotes the next state.

```
(automaton\1
  (q0
    (CONDITION HEAD-OPERATIONS NEXT-STATE)
    (CONDITION HEAD-OPERATIONS NEXT-STATE)
  )
  (q1
    (CONDITION HEAD-OPERATIONS NEXT-STATE)
    (CONDITION HEAD-OPERATIONS NEXT-STATE)
  )
)
```

There are four possible head operations.
1. Writing a symbol under the current tape head
2. Moving the tape head to the right
3. Moving the tape head to the left
4. Doing nothing on the tape

In the first case, the value to be written is used as the head operation. The value can be of any type.

```
(CONDITION "an example value" NEXT-STATE)
```

In the second case, "->" keyword is used to denote moving right.

```
(CONDITION -> NEXT-STATE)
```

In the third case, "<-" keyword is used to denote moving left.

```
(CONDITION <- NEXT-STATE)
```

In the last case, "." is used to denote doing nothing.

```
(CONDITION . NEXT-STATE)
```

In finite state machines, the tape is never modified, and the head can only move right. Therefore, all head operations are simply '->'

```
(automaton\1
  (q0
    (CONDITION -> NEXT-STATE)
    (CONDITION -> NEXT-STATE)
  )
  (q1
    (CONDITION -> NEXT-STATE)
    (CONDITION -> NEXT-STATE)
  )
)
```

If there are multiple tapes, the head operation for each tape must be given. An example automaton with 2 tapes:

```
(automaton\2
  (q0
    (CONDITION . . NEXT-STATE)
    (CONDITION <- -> NEXT-STATE)
  )
  (q1
    (CONDITION -> <- NEXT-STATE)
    (CONDITION "a" "b" NEXT-STATE)
  )
)
```

Next state can be one of the following
1. an identifier that denotes the name of the next state
2. keyword 'self'
3. keyword 'next'
4. keyword 'halt'
5. keyword 'accept'
6. keyword 'reject'

If an identifier is used as the next state name, the machine enters into that state when it follows the transition.

```
(automaton\1
  (q0
    (CONDITION -> q0)
    (CONDITION -> q1)
  )
  (q1
    (CONDITION -> q1)
    (CONDITION -> q0)
  )
)
```

The keyword 'self' can be used to enter the current state again.

```
(automaton\1
  (q0
    (CONDITION -> self)
    (CONDITION -> q1)
  )
  (q1
    (CONDITION -> self)
    (CONDITION -> q0)
  )
)
```

The keyword 'next' can be used to enter the state that follows the current state.

```
(automaton\1
  (q0
    (CONDITION -> self)
    (CONDITION -> next)
  )
  (q1
    (CONDITION -> self)
    (CONDITION -> q0)
  )
)
```

Using 'self' and 'next' makes it possible to write more "position independent" states.

**Halting**

In Theory Lisp, automatons can simulate both machines that compute by leaving a result on the tape and machines that accept or reject a given input.

'halt', 'accept', and 'reject' all terminate the execution of the machine, but their meanings are different. 

+ 'halt', means that the execution is done, and the computed result is on the tape.
+ 'accept' means that the execution is done, and the machine has decided that the input is accepted.
+ 'reject' means that the execution is done, and the machine has decided that the input is rejected.

This machine is the same as the previous one, except that it accepts if the end of input occurs in the q0 state, and it rejects if the end of input occurs in the q1 state.

```
(automaton\1
  (q0
    (CONDITION -> self)
    (CONDITION -> next)
    (END-OF-INPUT-CONDITION . accept)
  )
  (q1
    (CONDITION -> self)
    (CONDITION -> q0)
    (END-OF-INPUT-CONDITION . reject)
  )
)
```

**Transition Conditions**

A transition condition is a function that takes current symbols under tape heads as arguments, and returns a boolean value. When a PN expression is used as a transition condition, $1 refers to the current symbol on the first tape, $2 refers to the current symbol on the second tape, and so on.

The machine now does summation in modulo 2.

```
(automaton\1
  (q0
    ({= $1 0} -> self)
    ({= $1 1} -> next)
    (END-OF-INPUT-CONDITION . accept)
  )
  (q1
    ({= $1 0} -> self)
    ({= $1 1} -> q0)
    (END-OF-INPUT-CONDITION . reject)
  )
)
```

By using the implicit insertion feature of PN expressions, it is possible to get rid of $1's in this machine. Now, it looks more readable.

```
(automaton\1
  (q0
    ({= 0} -> self)
    ({= 1} -> next)
    (END-OF-INPUT-CONDITION . accept)
  )
  (q1
    ({= 0} -> self)
    ({= 1} -> q0)
    (END-OF-INPUT-CONDITION . reject)
  )
)
```

Finally, it is required to write a condition that recognizes the end of input. When the machine runs out of the initially given tape contents by moving right, it extends the tape by blank symbols. This is how infinite tapes of Turing machines are simulated. Therefore, it is reasonable to stop when a blank symbol is read from the tape. The blank symbol 'blank' is defined in automata.tl

```
(include "automata.tl")

(automaton\1
  (q0
    ({= 0} -> self)
    ({= 1} -> next)
    ({= blank} . accept)
  )
  (q1
    ({= 0} -> self)
    ({= 1} -> q0)
    ({= blank} . reject)
  )
)
```

+ Transitions conditions are tested in the order they appear.
+ At least one transition condition must be satisfied. If none of the transitions is satisfied, a runtime error is thrown because the machine must either halt or go to some other state. 

**State Outputs**

A state output is an optional PN expression that follows the state name. State outputs are executed when the machine enters a state. They can be used to display or extract information.

```
(include "automata.tl")
(include "util.tl")

(define DFA
  (automaton\1
    (q0 {println "Entered q0"}
        ({= 0} -> self)
        ({= 1} -> next)
        ({= blank} . accept))
    (q1 {println "Entered q1"}
        ({= 0} -> self)
        ({= 1} -> q0)
        ({= blank} . reject))
  )
)
```

**Transition Outputs**

A transition output is an optional PN expression that is executed when the machine follows a transition. Transition outputs can be used to display or extract information. They are written immediately after a transition.

```
(include "automata.tl")
(include "util.tl")

(define DFA
  (automaton\1
    (q0 {println "Entered q0"}
        ({= 0} -> self) {println "q0->q0"}
        ({= 1} -> next) {println "q0->q1"}
        ({= blank} . accept)
    )
    (q1 {println "Entered q1"}
        ({= 0} -> self) {println "q1->q1"}
        ({= 1} -> q0) {println "q1->q0"}
        ({= blank} . reject)
    )
  )
)
```

**Running an automaton**

An automaton expression evaluates to a procedure object just like lambdas and PN expressions. However, the input format of an automaton procedure is not arbitrary. An automaton takes tapes as arguments.

Each tape is a list whose first element is the head position and remaining elements are the tape contents. The leftmost symbol in the tape must be the left-end symbol. To run the above automaton, the following tape can be used. 'make-tape-noblank' from 'automata.tl' creates a tape with given contents.

```
(DFA (make-tape-noblank 0 1 1 0 1))
```

The output will be

```console
Entered q0
q0->q0
Entered q0
q0->q1
Entered q1
q1->q0
Entered q0
q0->q0
Entered q0
q0->q1
Entered q1
(cons -1 (cons (cons 6 (cons #<void> (cons 0 (cons 1 (cons 1 (cons 0 (cons 1 (cons null null)))))))) null))
```

The final line is the result of the machine. It contains the decision that the machine has made and the final tape contents of each tape. To extract these information easily, there are get-decision, get-tapes, get-head, get-contents, tape-tostring functions defined in "automata.tl"

```
; Run machine
(define machine-result
  (DFA (make-tape-noblank 0 1 1 0 1)))

; Extract results
(define decision (get-decision machine-result))
(define tape (car (get-tapes machine-result)))
(define head-position (get-head tape))
(define tape-contents (get-contents tape))

(println "Decision is " decision)
(println "Final head position is " head-position)
(println "Final tape contents : " (tape-tostring tape-contents))
```

The output will be

```console
Entered q0
q0->q0
Entered q0
q0->q1
Entered q1
q1->q0
Entered q0
q0->q0
Entered q0
q0->q1
Entered q1
Decision is -1
Final head position is 6
Final tape contents : #<void> 0 1 1 0 1 null
```

Decision being -1 means that the input is rejected as expected, because "0 + 1 + 1 + 0 + 1" is equal to 1 in modulo 2, which leaves the machine in the q1 state. If the machine had accepted the input, the decision would be +1. If the machine halts with "halt" keyword without making any decision, the decision becomes 0. The tape contents has not been modified except that a blank symbol (which is null) is inserted at the right end of the tape when the machine moved out of the initial tape contents, and the final tape head is on top of that null symbol.

**Composite Machines**

Using everything up to now, we can write complex finite state machines and DFAs, but this is not the preferred method of designing Turing machines even though it is possible. Instead of writing every single state and transition, composite machines can be constructed from simpler machines by using them as base machines.

If a state has a "base machine", it is written immediately after the state name with a colon. q0:R means that when the machine enters the q0 state, it first executes the machine R on the current tapes, and then starts executing the transitions of q0.

The previous machine is rewritten using the right moving machine 'R' (defined in automata.tl) as base machines of both states. All head operations are replaced with dots because they are already done by the base machine.

```
(include "automata.tl")
(include "util.tl")

(define DFA
  (automaton\1
    (q0:R {println "Entered q0"}
        ({= 0} . self) {println "q0->q0"}
        ({= 1} . next) {println "q0->q1"}
        ({= blank} . accept)
    )
    (q1:R {println "Entered q1"}
        ({= 0} . self) {println "q1->q1"}
        ({= 1} . q0) {println "q1->q0"}
        ({= blank} . reject)
    )
  )
)
```

One difference is that now the machine starts by moving to right immediately. Executing the transitions of q0 occurs afterwards. The previous tape format can no longer be used because the machine will skip the first digit on the tape. 

The solution is to use make-tape instead of make-tape-noblank. make-tape creates a tape that has the following format:
1. First symbol is the left-end symbol
2. Second symbol is a blank symbol
3. Remaining cells contain given tape contents.
4. Tape head is initially on the second symbol (the blank symbol)

```
; Run machine
(define machine-result
  (DFA (make-tape 0 1 1 0 1)))

; Extract results
(define decision (get-decision machine-result))
(define tape (car (get-tapes machine-result)))
(define head-position (get-head tape))
(define tape-contents (get-contents tape))

(println "Decision is " decision)
(println "Final head position is " head-position)
(println "Final tape contents : " (tape-tostring tape-contents))
```

The output will be

```console
Entered q0
q0->q0
Entered q0
q0->q1
Entered q1
q1->q0
Entered q0
q0->q0
Entered q0
q0->q1
Entered q1
Decision is -1
Final head position is 7
Final tape contents : #<void> null 0 1 1 0 1 null
```

The same decision is made. The tape head position is increased by one because of the additional blank symbol at the beginning, but it is still on the final blank symbol.

**Simplified Code**

The following is a simplified version that just runs the machine and gets the decision without any other output.

```
(include "automata.tl")

(define DFA
  (automaton\1
    (q0:R
        ({= 0} . self)
        ({= 1} . next)
        ({= blank} . accept))
    (q1:R
        ({= 0} . self)
        ({= 1} . q0)
        ({= blank} . reject))))

(println "Decision is " 
  (get-decision (DFA (make-tape 0 1 1 0 1))))
```

This is a very simple example used to demonstrate the usage of automatons. There is a Turing machine example in examples directory that shows how more complicated machines can be constructed.

**Halting in a base machine**

If a base machine halts normally using 'halt', the control is transferred to the outer machine and the outer machine continues running. However, if a base machine accepts or rejects, the execution of the outer machine is terminated, and the decision is returned to the original caller that called the outer machine. In the previous examples, the right moving machines do not make decisions, they just halt normally.

**Markov Chains**

An automaton can be used to simulate a Markov chain. The difference is that there are no tapes or head operations. The base machines and transition conditions are purely probabilistic. The arity of the automaton is 0, and the automaton is called without any arguments.

```
(include "automata.tl")
(include "util.tl")

; Total number of steps
(define STEPS 500)

; Number of visits for each state
(define A 0)
(define B 0)

; Stop condition
(define done?
  {= STEPS + A B})

; Automaton
(define MarkovChain
  (automaton\0
    (qA:P {(set! A (+ A 1))}
      (done? accept)
      ({p 0.3} qA)
      ({p 0.7} qB))
    (qB:P {(set! B (+ B 1))}
      (done? reject)
      ({p 0.2} qB)
      ({p 0.8} qA))))

; Run automaton
(let ((result (MarkovChain)))
  (println "Decision: " (get-decision result)))

; Print results
(let ((pA (* 100.0 (/ A STEPS)))
      (pB (* 100.0 (/ B STEPS))))
  (println "Visit Percentages: qA: " pA " qB: " pB))
```

When the machine enters a state, the base machine P gets executed, which generates a random number from standard uniform distribution in the background. Then the state output is called, so the number of visits of the corresponding counter is incremented. Finally, transition conditions are tested, and a next state is choosen based on the probability values. This process continues until the total number of steps becomes equal to STEPS. At the end, the percentages of visits of each state is printed.

The machine P and the function p are not special. They are part of the library and their definitions are in "automata.tl". Different probabilistic base machines and transition conditions can easily be designed. Moreover, it is also possible to design machines that both work on the tape and use probabilistic transitions, but you need write your own base machines and transition conditions to do this. P and p cannot be used if the machine uses tapes.

There is no mechanism to check whether the probabilities in a single state sum up to 1.0. In this example, both 0.3 + 0.7 = 1.0 and 0.2 + 0.8 = 1.0, but this is not enforced. It is programmers' responsibility. If the interpreter fails to find a transition that holds, an error is thrown, but this is not guaranteed to happen as transitions are random.

**Additional Features**

If a state with a base machine does not contain any transitions, it immediately goes to the next state after running the base machine. If the next state does not exist, the machine halts without a decision.

```
(automaton\1
  (q1:R) ; goes to q2 after moving right
  (q2:R) ; halts after moving right
)
```

## Macros

Macros are functions that transform an expression with a user-defined syntax to a known type of expression. For example, the 'defun' macro defined in "syntax.tl" transforms the expression

```
(defun (f x y)
  (+ x y))
```

into the following expression

```
(define f
  (lambda (x y)
    (+ x y)))
```

The syntax for using a macro is the same as Scheme, but the syntax for defining a macro is significantly different from other Lisp-like languages. Since Theory Lisp source code is represented as a string, the input to a macro is an arbitrary token stream. A macro reads tokens one by one and generates new source code. The keyword "define-syntax" is used to a define a new macro.

There are 4 builtin functions that macros can use to access the token stream
+ **peek-tkn** returns the current token on the stream, but does not change the position on the stream.
+ **pop-tkn** return the current token on the stream, and increments the position.
+ **prev-tkn** returns the previous token on the stream, and decrements the position.
+ **parse** returns the next entire expression that appear on the token stream. The position is set to the first token after the end of the expression.

**Example 1**

```
(define-syntax println-macro
  (lambda (tkns)
    (strcat "(display " (pop-tkn tkns) "(newline))")
  )
)

(println-macro 123)
```

Initially, the parser reads the define-syntax expression. It evaluates the body of the expression to a lambda object, and saves it in the global stack frame as a macro. Nothing else happens. No meaningful value is returned.

When the parser reaches the last line, it recognizes the macro invocation and calls the macro with the current state of the token stream. The builtin function pop-tkn "pops" the token 123. Then, by using the returned token, the function generates the string

```
"(display 123 (newline))"
```

and returns it back to the parser.

The parser receives the string, and parses it again. In the end, a function evaluation expression is constructed and appended to the parse tree.

**Example 2**

The macro 'let1' is a version of 'let' that defines only a single local variable. 

```
(define-syntax let1
   (lambda (tkns)
     (let ((leftp (pop-tkn tkns))
           (name (pop-tkn tkns))
           (value (parse tkns))
           (rightp (pop-tkn tkns))
           (body (parse tkns)))
       (strcat "(let ((" name " " value " )) " body ")"))))

(let1 (a 1) (display a (newline)))
```

For comparison, there is a similar example for Scheme in [Guile reference](https://www.gnu.org/software/guile/manual/html_node/Syntax-Rules.html). Unfortunately, the ability to parse arbitrary expressions instead of lists increases complexity of macros. 

**Recursion in macros**

It is possible to use previously defined macros inside a later macro definition, but a macro is not directly available to itself. The new syntax defined by the macro cannot be used inside the macro's own definition. However, there is still a way to implement recursion in macros. Like M4 macro language, the result of a macro is rescanned, and it can contain a macro invocation that refers to the macro itself. The following example generates code that computes factorial.

**Example 3**

```
(define-syntax fact-macro
  (lambda (tkns)
    (let ((arg (eval (pop-tkn tkns))))
      (if (= arg 0)
        "1"
        (strcat
          "(* " arg " (fact-macro " (- arg 1) ")))")))))
```

Example trace of the macro invocation (fact-macro 5) and resulting substitutions

```
(fact-macro 5)
(* 5 (fact-macro 4))
(* 5 (* 4 (fact-macro 3)))
(* 5 (* 4 (* 3 (fact-macro 2))))
(* 5 (* 4 (* 3 (* 2 (fact-macro 1)))))
(* 5 (* 4 (* 3 (* 2 (* 1 (fact-macro 0))))))
(* 5 (* 4 (* 3 (* 2 (* 1 1)))))
```

When the resulting expression gets evaluated at runtime, it yields 120. The equivalent macro in M4 is

```M4
define(`factorial', `ifelse(`$1', `0', `1', `($1 * factorial(eval($1 - 1)))')')dnl
factorial(`5')
```

This is intended to be a simple example for demonstrating recursion in macros. It is possible to compute factorial in a macro without making the macro itself recursive. There are also better ways to write the macro if a recursive approach is choosen.

Macros can do everything that ordinary functions can do, including displaying values and doing arbitrary IO operations. The error messages resulting from macro invocations are rarely helpful, so displaying strings can be used as a method to debug macros.

**Macros based on pattern matching**

The macros previously described are powerful and can parse any user-defined expression even if the expression contains unbalanced parenthesis. However, this requires complicated code written in an imperative style to directly access the token stream. In most use cases of macros, the ability to parse such complex expressions is unnecessary, and the described macros are inconvenient compared to pattern matching macros of other Lisp dialects. As an alternative, Theory Lisp also provides macros that are based on pattern matching.

A pattern matching macro is defined using 'syntax-rules' which is itself a complex macro in "match.tl" that yields a lambda object. The main limitation of these kind of macros is that they can only contain balanced pairs of parenthesis (), [], {} in macro invocations. For example, (some-macro ([1 2 3] {3 4 (5 6)})) can be a valid invocation, but (some-macro {[}) cannot. Also, string literals are currently not allowed.

**Example 4**

```
(define-syntax let1
  (syntax-rules ()
    (
      (let1 (name value) body) ; invocation syntax
      (let ((name value)) body) ; transcription syntax
    )
  )
)

(let1 (x 3) (+ x 4))
```

This macro is equivalent to the previous 'let1' macro written in imperative style. Since it does not require any unbalanced pairs of parenthesis, syntax-rules is suitable for this macro. The first expression in the syntax-rules body is called the macro invocation syntax, and the second expression is called the macro transcription syntax. When the given macro invocation matches the macro invocation syntax, the identifiers declared in the invocation syntax are assigned values, and these values are substituted into the transcription syntax.

In this example, name is assigned to x, value is assigned to 3, and body is assigned to the expression (+ x 4). The generated code is

```
(let ((x 3)) (+ x 4))
```

which can be verified by running the code in verbose mode.

Warning: In many List dialects, [] and {} are aliases for (). In Theory Lisp, they are recognized as distinct types of parenthesis, and cannot be used interchangeably. If a macro invocation syntax contains {}, using () won't match.

**Example 6**

It is possible to write a simple defun macro using pattern matching.

```
(include "util.tl")

(define-syntax defun2
  (syntax-rules (\)
    ((defun2 (func-name) body)
     (define func-name (lambda () body)))
    ((defun2 (func-name args ...) body)
     (define func-name (lambda (args ...) body)))
  )
)

(defun2 (func)
  (println "zero arguments"))
(func)

(defun2 (func x)
 (println (+ x 1)))
(func 2)
```

If a pattern variable is followed by an ellipsis in the invocation syntax, it matches with one or more expressions up to the first closing parenthesis. They are expanded in the transcription syntax by using an ellipsis again.

**Example 7**

```
(define-syntax example
  (syntax-rules ()
    ((example (H T ...) ...)
     (cons (list H ...) (list T ...)))))

(example (2 -1) (3 0) (4 1) (5 2))
```

If an entire parenthesized expression is followed by an ellipsis in the invocation syntax, it also matches with one or more expressions of the same form. In this example, (H T ...) is repeatedly matched with given parenthesized pairs. Inside each expression, H matches the head, and T ... matches the tail. Therefore, the expression yields 

```
(cons (list 2 3 4 5) (list -1 0 1 2)
```

**Example 8**

It is possible to write ellipsis after a parenthesized expression inside the transcription syntax. The entire expression will be repeated until the pattern variables that requires expansion are completely expanded.

```
(define-syntax example
  (syntax-rules ()
    ((example H T ...)
     (list (cons H T) ...))))

(example 1 2 3 4 5)
; yields (list (cons 1 2) (cons 1 3) (cons 1 4) (cons 1 5))
```

**define-constexpr** and **constexpr**

The interpreter starts after the parse tree has been fully generated, which means that all macro invocations happen before the interpreter starts. Consequently, ordinary definitions are not available inside a macro definition. The following code will cause a parser error.

```
(define factorial
  (lambda (x)
    (if (= x 0)
      1
      (* x (factorial (- x 1))))))

(define-syntax macro-factorial
  (lambda (tkns)
    (strcat (factorial (eval (pop-tkn tkns))))))

(macro-factorial 5)
```

When macro-factorial is invoked, the first define expression has not been interpreted, and there is no 'factorial'. In order to define 'factorial' as an object available to macros, it must be defined using "define-constexpr".

```
(define-constexpr factorial
  (lambda (x)
    (if (= x 0)
      1
      (* x (factorial (- x 1))))))

(define-syntax macro-factorial
  (lambda (tkns)
    (strcat (factorial (eval (pop-tkn tkns))))))

(macro-factorial 5)
```

"constexpr" is used to do computation at parsing time. The multiplication of 3 and 4 does not require any runtime information, so it can be done at parsing time.

```
(define f
  (lambda (x)
    (+ x (constexpr (* 3 4)))))

(f 6)
```

When this code is run in verbose mode (with -v option as command line parameter) the result is the following:

```
Expression: (define f (lambda (x) (+ x 12)))
Result: (void)
Expression: (f 6)
Result: 18
```

The first expression contains the result of the multiplication, and there is no multiplication happening at runtime.

define-constexpr and constexpr are themselves macros defined in "syntax.tl". They use builtin eval function to create this behaviour. 

## Try-Catch Expression

The try-catch expression, together with the builtin function "error" provides a very naive method to create and handle exceptions.

```
(try 
  (begin
    (display 1)
    (error "some error")
    (display 2))
  ) (catch (e)
    (display e)))
```

'(e)' in the beginning of the catch block defines a local variable named 'e', which is only visible inside the handler expression. It stores the string message contained in the caught error.

If no exception occurs, the return value of a try-catch expression is equal to the return value of the try block. Otherwise, it yields the return value of the catch block.

When an exception occurs, the execution of the try block terminates immediately, meaning that 2 is not printed. Instead, catch block catches the error and prints its value "some error" to the screen.

## While Expression

'while' is a macro defined in the library that simulates an imperative 'while' loop with recursion.

**Example**

```
(include "macro.tl")

(define i 0)
(while (!= i 10) (begin
  (println i)
  (set! i (+ i 1))))
```

## Assert Expression

'assert' is a macro defined in the library that throws an error if the expression given as argument yields #f. It prints an "Assertion failure" message together with the expression that returned false.

```
(assert (!= 0 1))
```

This expression throws an error with message "Assertion failure: (!= 0 1)"