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

(include "list.tl")
(include "string.tl")

; An instance of void
(define voidobj (define voidobj null))

; Takes unlimited arguments and returns void
(define void
  (lambda (...)
    voidobj))

; Sequencing
(define begin
  (lambda (...) (last va_args)))
(define begin0
  (lambda (...) (car va_args)))

; Returns true if given number is zero (common base case in recursion)
(define zero?
  (lambda (x)
    (= x 0)))
; Return true if given number is one (common base case in recursion)
(define one?
  (lambda (x)
    (= x 1)))
; Return true if given number is two (common base case in recursion)
(define two?
  (lambda (x)
    (= x 2)))


; Used as last condition expression in cond expressions
(define else #t)

; One level of currying
(define cur
  (lambda (f)
    (lambda [f] (x)
      (lambda [f x] (...)
        (f x &va_args))
    )
  )
)

; Wraps function inside another function.
(define wrap
  (lambda (f)
    (lambda [f] () f)))

; Finds minimum in a list
(define min
  (lambda (lst)
    (reduce (lambda (a b) (if (< a b) a b)) lst)))

; Finds maximum in a list
(define max
  (lambda (lst)
    (reduce (lambda (a b) (if (>= a b) a b)) lst)))

; Applies a function repeatedly
(define rep
  (lambda (n func value)
    (if (zero? n)
      value
      (rep (- n 1) func (func value)))))

; Prints string with a newline at the end
(define println
  (lambda\1 (...)
    (display &va_args nl)))
