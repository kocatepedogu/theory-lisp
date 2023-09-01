# Theory Lisp

Theory Lisp is a simple language for simulating theoretical models of computation. Its syntax and functionality are similar to Lisp, but it is not truely a Lisp dialect. 

The language is dynamically typed, dynamically scoped, and fully interpreted. Code can be represented as data, and it is possible to dynamically generate and execute code at runtime. The interpeter uses a recursive descent parser. Arbitrary user-defined expressions (even expressions that does not have a list form) can be used by extending the parser with macros. Both imperative macros and pattern matching macros are available.

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