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

; This file defines collection operations for lists, dictionaries, sets and stacks.

(include "macro.tl")
(include "util.tl")

; ------------------------------------------------------------------------------
; List Operations
; ------------------------------------------------------------------------------

; Returns true if the given object is a proper list
(define-constexpr list?
  (lambda (lst)
    (if (pair? lst)
      (list? (cdr lst))
      (null? lst)
    )
  )
)

; Syntactic sugar for (car (cdr lst))
(define-constexpr cadr (lambda (lst) (car (cdr lst))))

; Syntactic sugar for (cdr (cdr lst))
(define-constexpr cddr (lambda (lst) (cdr (cdr lst))))

; Returns the last element of the list
(define-constexpr last
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
(define-constexpr length
  (lambda (lst)
    (if (null? lst)
      0
      (+ 1 (length (cdr lst))))))


; Concatenates given lists
(define-constexpr append
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
(define-constexpr times
  (lambda (n lst)
    (if (zero? n)
      null
      (append lst (times (- n 1) lst))
    )
  )
)

; Creates a list of n zeros
(define-constexpr zeros (lambda (n) (times n (list 0))))

; Creates a list of n ones
(define-constexpr ones (lambda (n) (times n (list 1))))

; Returns index of the given element in the list
(define-constexpr indexof
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
(define-constexpr at
  (lambda (index lst)
    (let ((helper (lambda (lst index original-index)
      (if (and (list? lst) (not (null? lst)))
        (if (zero? index)
          (car lst)
          (helper (cdr lst) (- index 1) original-index))
        (error (strcat "Index " original-index " is out of bounds"))))))
      (helper lst index index))))

; True if key is in the list
(define-constexpr list-contains
  (lambda (lst key)
    (if (null? lst)
      #f
      (if (= (car lst) key)
        #t
        (list-contains (cdr lst) key)))))

; Applies given function to each element of the list
(define-constexpr map
  (lambda (func lst)
    (if (null? lst)
      null
      (list (func (car lst)) %(map func (cdr lst)))
    )
  )
)

; Applies given function to each element of the list to obtain a scalar
(define-constexpr reduce
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
(define-constexpr min
  (lambda (lst)
    (reduce (lambda (a b) (if (< a b) a b)) lst)))

; Finds maximum in a list
(define-constexpr max
  (lambda (lst)
    (reduce (lambda (a b) (if (>= a b) a b)) lst)))

; ------------------------------------------------------------------------------
; Dictionary Operations
; ------------------------------------------------------------------------------

(define-constexpr dict-put-helper
  (lambda (dict key value)
    (if (null? dict)
      (list (cons key value))
      (let ((first-pair (car dict)))
        (let ((existing-key (car first-pair))
              (existing-value (cdr first-pair)))
          (if (= existing-key key)
            (cons (cons key value) (cdr dict))
            (cons (cons existing-key existing-value) (dict-put-helper (cdr dict) key value))))))))

(define-syntax dict-put
  (lambda (tkns)
    (let ((dictionary (parse tkns))
          (leftp (pop-tkn tkns))
          (key (parse tkns))
          (value (parse tkns))
          (rightp (pop-tkn tkns)))
      (strcat "(dict-put-helper "dictionary" "key" "value")"))))

(define-syntax dict-put!
  (lambda (tkns)
    (let ((dictionary-name (pop-tkn tkns))
          (leftp (pop-tkn tkns))
          (key (parse tkns))
          (value (parse tkns))
          (rightp (pop-tkn tkns)))
      (strcat "(set! " dictionary-name " (dict-put " dictionary-name " (" key " " value ")))"))))

(define-syntax dict-putall
  (lambda (tkns)
    (let ((dictionary (parse tkns))
          (other (parse tkns)))
      (strcat "(list %" dictionary " %" other ")"))))

(define-syntax dict-putall!
  (lambda (tkns)
    (let ((dictionary-name (pop-tkn tkns))
          (other (parse tkns)))
      (strcat "(set! " dictionary-name " (dict-putall " dictionary-name " " other "))"))))

(define-constexpr dict-get
  (lambda (lst key)
    (if (null? lst)
      (void)
      (let ((first-pair (car lst)))
        (let ((first-key (car first-pair)))
          (if (= first-key key)
            (cdr first-pair)
            (dict-get (cdr lst) key)))))))

; ------------------------------------------------------------------------------
; Set Operations
; ------------------------------------------------------------------------------

(define-constexpr set-put
  (lambda (lst key)
    (if (list-contains lst key)
      lst
      (cons key lst))))

(define-syntax set-put!
  (lambda (tkns)
    (let ((set-name (pop-tkn tkns))
          (value (parse tkns)))
      (strcat
        "(set! " set-name " (set-put " set-name " " value "))"))))

(define-constexpr set-putall
  (lambda (lst ...)
    (if (null? va_args)
      lst
      (set-put (set-putall lst %(cdr va_args)) (car va_args)))))

(define-syntax set-putall!
  (lambda (tkns)
    (let ((set-name (pop-tkn tkns))
          (value (parse tkns)))
      (begin
        (while (!= (peek-tkn tkns) ")")
          (strcat! value (strcat " " (parse tkns))))
        (strcat "(set! " set-name " (set-putall " set-name " " value "))")))))

; ------------------------------------------------------------------------------
; Stack Operations
; ------------------------------------------------------------------------------

(define-syntax push!
  (lambda (tkns)
    (let ((stack-name (pop-tkn tkns))
          (value (parse tkns)))
      (strcat "(set! " stack-name " (cons " value " " stack-name "))"))))

(define-syntax pop!
  (lambda (tkns)
    (let ((stack-name (pop-tkn tkns)))
      (strcat
        "(let ((result (car " stack-name ")))
           (begin
             (set! " stack-name " (cdr " stack-name "))
             result))"))))

