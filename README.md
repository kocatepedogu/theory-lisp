# Theory Lisp

Theory Lisp is a Lisp-like language designed for experimenting with theoretical models of computation and programming language features.

In addition to commonly used Lisp forms, Theory Lisp provides special syntax for designing single tape and multiple tape Turing machines. This syntax can easily be used for other types of machines, such as finite state machines and algorithmic state machines.

The language is dynamically typed and dynamically scoped. Currently, there are no references or pointers, and function arguments are pass-by-value.

## Installation 

On Linux, macOS and FreeBSD following commands compile and install Theory Lisp

```console
autoreconf --install
./configure
make
make install
make check
```

It is recommended to use --prefix option in configure step to specify an installation directory that does not require root access.

## Usage

To start read-evaluate-print loop (REPL) without opening any program, run

```console
tlisp
```

To execute a Theory Lisp program, run
```console
tlisp code.tl
```
If the program does not explicitly exit at the end, the interpreter will start REPL. All global variables defined by the program are accessible in the REPL.

If REPL is not needed, use,

```console
tlisp code.tl -x
```
The -x option disables REPL, so that the interpreter always exits when the program finishes.



