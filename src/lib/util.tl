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

; This file provides frequently needed language features.

(include "macro.tl")

; Returns true if given number is zero (common base case in recursion)
(define-constexpr zero?
  (lambda (x)
    (= x 0)))
; Return true if given number is one (common base case in recursion)
(define-constexpr one?
  (lambda (x)
    (= x 1)))
; Return true if given number is two (common base case in recursion)
(define-constexpr two?
  (lambda (x)
    (= x 2)))

; Used as last condition expression in cond expressions
(define-constexpr else #t)

; One level of currying
(define-constexpr cur
  (lambda (f)
    (lambda [f] (x)
      (lambda [f x] (...)
        (f x %va_args))
    )
  )
)

; Wraps function inside another function.
(define-constexpr wrap
  (lambda (f)
    (lambda [f] () f)))

; Applies a function repeatedly
(define-constexpr rep
  (lambda (n func value)
    (if (zero? n)
      value
      (rep (- n 1) func (func value)))))

; Prints string with a newline at the end
(define-constexpr println
  (lambda\1 (...)
    (display %va_args (newline))))

