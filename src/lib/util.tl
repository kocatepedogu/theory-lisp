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

(include "syntax.tl")

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
        (f x %va_args))
    )
  )
)

; Wraps function inside another function.
(define wrap
  (lambda (f)
    (lambda [f] () f)))

; Applies a function repeatedly
(define rep
  (lambda (n func value)
    (if (zero? n)
      value
      (rep (- n 1) func (func value)))))

; Prints string with a newline at the end
(define println
  (lambda\1 (...)
    (display %va_args (newline))))

; Returns true if the given object is a proper list
(define list?
  (lambda (lst)
    (if (pair? lst)
      (list? (cdr lst))
      (null? lst)
    )
  )
)

; Syntactic sugar for (car (cdr lst))
(define cadr (lambda (lst) (car (cdr lst))))

; Syntactic sugar for (cdr (cdr lst))
(define cddr (lambda (lst) (cdr (cdr lst))))

; Returns the last element of the list
(define last
  (lambda (lst)
    (if (null? lst)
      (error "List has no elements")
      (if (list? lst)
        (let ((rest (cdr lst)))
          (if (null? rest)
            (car lst)
            (last rest)
          )
        )
        (error "Given object is not a list")
      )
    )
  )
)

; Returns the length of the given list
(define length
  (lambda (lst)
    (if (null? lst)
      0
      (+ 1 (length (cdr lst))))))


; Concatenates given lists
(define append
  (lambda (...)
    (let ((len (length va_args)))
      (cond ((= len 0) null)
            ((= len 1) (car va_args))
            (else (list %(car va_args) %(append %(cdr va_args))))
      )
    )
  )
)

; Multiplies list
(define times
  (lambda (n lst)
    (if (zero? n)
      null
      (append lst (times (- n 1) lst))
    )
  )
)

; Creates a list of n zeros
(define zeros (lambda (n) (times n (list 0))))

; Creates a list of n ones
(define ones (lambda (n) (times n (list 1))))

; Returns index of the given element in the list
(define indexof
  (lambda (element lst)
    (if (null? lst)
      (error (strcat "The list does not contain the element " element))
      (if (= (car lst) element)
        0
        (let ((inner-index (indexof element (cdr lst))))
          (if (null? inner-index)
              null
              (+ 1 inner-index)
          )
        )
      )
    )
  )
)

; Returns element at the given index
(define at
  (lambda (index lst)
    (let ((helper (lambda (lst index original-index)
      (if (and (list? lst) (not (null? lst)))
        (if (zero? index)
          (car lst)
          (helper (cdr lst) (- index 1) original-index))
        (error (strcat "Index " original-index " is out of bounds"))))))
      (helper lst index index))))

; Applies given function to each element of the list
(define map
  (lambda (func lst)
    (if (null? lst)
      null
      (list (func (car lst)) %(map func (cdr lst)))
    )
  )
)

; Applies given function to each element of the list to obtain a scalar
(define reduce
  (lambda (func lst)
    (let ((len (length lst)))
      (cond
        ((zero? len)
          (error "cannot reduce empty list"))
        ((one? len)
          (car lst))
        ((two? len)
          (func (car lst) (cadr lst)))
        (else
          (func (car lst) (reduce func (cdr lst))))
      )
    )
  )
)

; Finds minimum in a list
(define min
  (lambda (lst)
    (reduce (lambda (a b) (if (< a b) a b)) lst)))

; Finds maximum in a list
(define max
  (lambda (lst)
    (reduce (lambda (a b) (if (>= a b) a b)) lst)))
