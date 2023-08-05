(include "util.tl")

; Left end symbol
(define left-end (void))

; Blank symbol
(define blank null)

; Creates a tape
(define make-tape
  (lambda (...)
    (cons 1 (list left-end blank &va_args))))

; Returns string representation of a tape
(define tape-tostring
  (lambda (tape)
    (if (null? tape)
      ""
      (strcat (car tape) " " (tape-tostring (cdr tape)))
    )
  )
)

; Gets decision from a machine result
(define get-decision car)
; Gets tape results from a machine result
(define get-tapes cdr)
; Get head position from a tape result
(define get-head car)
; Get tape contents from a tape result
(define get-contents cdr)

; Writing machine
(define W
  (lambda (value)
    (automaton\1 [value]
      (q0 ({#t} value halt)))))

; Right moving machine
(define R
  (automaton\1
    (q0 ({#t} -> halt))))

; Left moving machine
(define L
  (automaton\1
    (q1 ({#t} <- halt))))

; Stops on the first blank on right
(define blankOnR
  (automaton\1
    (q1:R ({= blank} . halt)
          ({!= blank} . self))))

; Stops on the first blank on left
(define blankOnL
  (automaton\1
    (q1:L ({= blank} . halt)
          ({!= blank} . self))))

; Stops on the first non-blank on right
(define nonBlankOnR
  (automaton\1
    (q1:R ({= blank} . self)
          ({!= blank} . halt))))

; Stops on the first non-blank on left
(define nonBlankOnL
  (automaton\1
    (q1:L ({= blank} . self)
          ({!= blank} . halt))))

; Stops on the first x on right
(define onR
  (lambda (x)
    (automaton\1 [x]
      (q1:R ({= x} . halt)
            ({!= x} . self)))))

; Stops on the first non-x on right
(define nonR
  (lambda (x)
    (automaton\1 [x]
      (q1:R ({!= x} . halt)
            ({= x} . self)))))

; Stops on the first x on left
(define onL
  (lambda (x)
    (automaton\1 [x]
      (q1:L ({= x} . halt)
            ({!= x} . self)))))

; Stops on the first non-x on left
(define nonL
  (lambda (x)
    (automaton\1 [x]
      (q1:L ({!= x} . halt)
            ({= x} . self)))))
