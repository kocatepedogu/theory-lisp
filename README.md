# Theory Lisp

Theory Lisp is a simple language for simulating theoretical models of computation. Its syntax and functionality are similar to Lisp, but it is not truely a Lisp dialect. 

The language is dynamically typed, dynamically scoped, and fully interpreted. Code can be represented as data, and it is possible to dynamically generate and execute code at runtime. The interpeter uses a recursive descent parser. Arbitrary user-defined expressions (even expressions that does not have list form) can be used by extending the parser with macros. Both imperative macros and pattern matching macros are available.

In addition to functional and imperative Lisp forms, Theory Lisp provides special syntax for designing automatons. The syntax can be used for designing finite state machines, single-tape or multi-tape Turing machines and (very simple) Markov chains. Composite machines can be constructed from simpler machines by using the smaller machines as base machines. The goal of the language is to be a self-contained educational tool for simulating automatons. The further development will be on adding different models of computation.

## Dependencies

The interpreter does not require any external libraries, but Check is needed for unit tests. 

On Debian based systems,

```console
sudo apt install libcheck
```

On BSD,
```console
pkg install check
```

On Mac OS X (only tested on very old OS X 10.5 PPC with tigerbrew), 
```console
brew install check
```

## Installation 

On Linux, BSD and macOS/Mac OSX following commands will compile and install Theory Lisp

```console
autoreconf --install
./configure
make
make install
make check
```

It is recommended to use --prefix option in configure step to specify an installation directory that does not require root access.

## Usage

To start read-evaluate-print loop (REPL) without opening any program

```console
tlisp
```

To execute a Theory Lisp program

```console
tlisp code.tl
```

If the program does not explicitly exit at the end, the interpreter will start REPL. All global variables defined during the execution of the program are accessible in the REPL.

If REPL is not needed, use,

```console
tlisp code.tl -x
```

The -x option disables REPL, so that the interpreter always exits when the program finishes.

REPL currently does not support entering multi-line code.

## Example Code

**Summation in modulo 2 using a finite automaton**

```scheme
(include "util.tl")
(include "automata.tl")

(define mod2sum
  (automaton\1
    (q0:R {(set! result 0)}
      ({= 0} . q0)
      ({= 1} . q1)
      ({= blank} . halt))

    (q1:R {(set! result 1)}
      ({= 0} . q1)
      ({= 1} . q0)
      ({= blank} . halt))
  )
)

(let ((result -1))
  (begin
    (mod2sum (make-tape 1 1 0 1))
    (println "Result:" result)))
```

**Markov chain with 2 states**

```scheme
(include "automata.tl")
(include "util.tl")

; Total number of steps
(define STEPS 1000)

; Number of visits for each state
(define A 0)
(define B 0)

; Stop condition
(define done?
  {= STEPS + A B})

; Definiton of the automaton
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

There is a more complex Turing machine example together with a detailed state diagram in the examples directory. The examples here are just for demonstrating the general syntax. Pattern matching macro examples are also available in the examples.
