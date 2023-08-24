# Theory Lisp

Theory Lisp is a Lisp-like language designed for experimenting with theoretical models of computation.

The language is dynamically typed, dynamically scoped, and fully interpreted. Code can be represented as data, and it is possible to dynamically generate and execute code at runtime. The interpreter uses a recursive descent parser.

In addition to commonly used Lisp forms, Theory Lisp provides special syntax for designing automatons. The syntax can be used for designing finite state machines, single-tape or multi-tape Turing machines and (very simple) Markov chains. Composite machines are constructed from simpler machines by using the smaller machines as base machines.

Theory Lisp is aimed to be used for educational purposes. The language is still incomplete, and the implementation is not stable enough.

## Dependencies

The interpreter does not require any external libraries, but Check is needed for unit tests. To install Check on Debian based systems,

```console
sudo apt install libcheck
```

## Installation 

On Linux, following commands will compile and install Theory Lisp

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