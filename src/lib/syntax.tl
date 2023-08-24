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

; Syntax: (inc! x)
; increments a variable
(define-syntax inc!
  (lambda (tkns)
    (let ((var-name (pop-tkn tkns)))
      (strcat "(set! " var-name " (+ 1 " var-name "))"))))

; Syntax: (dec! x)
; decrements a variable
(define-syntax dec!
  (lambda (tkns)
    (let ((var-name (pop-tkn tkns)))
      (strcat "(set! " var-name " (- " var-name " 1))"))))

; Syntax: (while condition body)
; body is executed only for side effects. The return value is always void.
(define-syntax while
  (lambda (tkns) (strcat
    "(let ((loop-func
              (lambda ()
                (if " (parse tkns)
                  "(begin "
                    (parse tkns)
                    "(loop-func)) "
                  "(void)))))
            (loop-func))")))

; Syntax: (strcat! x ...)
; imperative version of strcat that appends to the given string
(define-syntax strcat!
  (lambda (tkns)
    (let ((var-name (pop-tkn tkns))
          (result ""))
      (begin
        (while (!= (peek-tkn tkns) ")")
          (set! result (strcat result (parse tkns))))
        (strcat "(set! " var-name " (strcat " var-name " " result "))")))))

; Syntax: (defun (func-name arguments ...) body)
; Syntactic sugar for defining lambdas.
(define-syntax defun
  (lambda (tkns)
  (begin
    ; Pop left parenthesis
    (if (!= (pop-tkn tkns) "(")
      (error "Missing left parenthesis") (void))
    ; Get function name
    (set! func-name (pop-tkn tkns))
    ; Get function arguments
    (set! func-args "")
    (while (!= (peek-tkn tkns) ")")
      (strcat! func-args " " (pop-tkn tkns)))
    ; Pop right parenthesis
    (pop-tkn tkns)
    ; Parse body
    (set! body (parse tkns))
    ; Construct result
    (strcat
      "(define " func-name
        " (lambda ( " func-args " ) "
            body "))"))))

; Syntax: (and arg1 arg2 arg3 ...)
; Takes zero or more arguments. Result is #t if no arguments are given.
; Short circuiting boolean AND operator
(define-syntax and
  (lambda (tkns)
    (let ((conditions "")
          (count 0))
      (begin
        (while (!= (peek-tkn tkns) ")") (begin
          (strcat! conditions "(if " (parse tkns) " ")
          (inc! count)))
        (set! conditions (strcat conditions "#t"))
        (while (!= count 0) (begin
          (strcat! conditions " #f)")
          (dec! count)))
        conditions))))

; Syntax: (or arg1 arg2 arg3 ...)
; Takes zero or more arguments. Result is #f is no arguments are given.
; Short circuiting boolean OR operator
(define-syntax or
  (lambda (tkns)
    (let ((conditions "")
          (count 0))
      (begin
        (while (!= (peek-tkn tkns) ")") (begin
          (strcat! conditions "(if (not " (parse tkns) ") ")
          (inc! count)))
        (set! conditions (strcat conditions "#f"))
        (while (!= count 0) (begin
          (strcat! conditions " #t)")
          (dec! count)))
        conditions))))
