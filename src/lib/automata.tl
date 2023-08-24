;
; Copyright 2023 Dogu Kocatepe
; This file is part of Theory Lisp Library.

; Theory Lisp Library is free software: you can redistribute it
; and/or modify it under the terms of the GNU Lesser General Public License
; as published by the Free Software Foundation, either version 3 of
; the License, or (at your option) any later version.

; Theory Lisp Library is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
; See the GNU Lesser General Public License for more details.
;
; You should have received a copy of the GNU Lesser General Public License
; along with Theory Lisp Library. If not, see <https://www.gnu.org/licenses/>.
;

(include "util.tl")

; Left end symbol
(define left-end (void))

; Blank symbol
(define blank null)

; Creates a tape
(defun (make-tape ...)
  (cons 1 (list left-end blank %va_args)))

; Creates a tape that does not have blank after left-end
(defun (make-tape-noblank ...)
  (cons 1 (list left-end %va_args)))

; Returns string representation of a tape
(defun (tape-tostring tape)
  (if (null? tape)
    ""
    (strcat (car tape) " " (tape-tostring (cdr tape)))
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
(defun (W value)
  (automaton\1 [value]
    (q0 ({#t} value halt))))

; Right moving machine
(define R
  (automaton\1
    (q0 ({#t} -> halt))))

; Left moving machine
(define L
  (automaton\1
    (q1 ({#t} <- halt))))

; Uniform random number generating machine
(define P
  (automaton\0
    (q1 {(define _rand_ (random))}
      ({#t} halt))))

; Used as a probabilistic transition condition
(defun (p probability)
  (if (< _rand_ probability)
    #t
    (begin
      (define _rand_ (- _rand_ probability))
      #f)))

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
(defun (onR x)
    (automaton\1 [x]
      (q1:R ({= x} . halt)
            ({!= x} . self))))

; Stops on the first non-x on right
(defun (nonR x)
  (automaton\1 [x]
    (q1:R ({!= x} . halt)
          ({= x} . self))))

; Stops on the first x on left
(defun (onL x)
  (automaton\1 [x]
    (q1:L ({= x} . halt)
          ({!= x} . self))))

; Stops on the first non-x on left
(defun (nonL x)
  (automaton\1 [x]
    (q1:L ({!= x} . halt)
          ({= x} . self))))
