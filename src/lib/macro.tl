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

; This file defines frequently used macros.

; Computes a value during parsing (before runtime)
(define-syntax constexpr
  (lambda (tkns)
    (strcat
      (eval (strcat (parse tkns))))))

; Defines a variable during parsing (before runtime)
(define-syntax define-constexpr
  (lambda (tkns)
    (let ((name (pop-tkn tkns)))
      (let ((define-expr (strcat "(define " name " " (parse tkns) " )")))
        (begin (eval define-expr) "(void)")))))

; Makes an assertion check. If the assertion fails, an error containing the cause is thrown.
(define-syntax assert
  (lambda (tkns)
    (let ((con (parse tkns))
          (remove-quotes
            (lambda (str)
              (if (= str "") ""
                (if (= (strcar str) (quotation-mark))
                  (strcat " " (remove-quotes (strcdr str)))
                  (strcat (strcar str) (remove-quotes (strcdr str))))))))
      (strcat
        "(if (not " con ")
           (error (strcat "
             (quotation-mark)
               "Assertion failure: " (remove-quotes con)
             (quotation-mark) "))
           (void))"))))

; Imperative while loop
; Syntax: (while condition body)
; Body is executed only for side effects as long as the condition is true.
; The return value is always void.
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

; An imperative version of if
; The body can contain one or more expressions. There is no else case.
; Syntax: (when condition body ...)
; value of the last expression in the body is returned. if condition is false, return value is void.
(define-syntax when
  (lambda (tkns)
    (let ((condition-expr (parse tkns))
          (body-expr (parse tkns)))
      (begin
        (while (!= (peek-tkn tkns) ")")
          (set! body-expr (strcat body-expr (parse tkns))))
        (strcat
           "(if " condition-expr " (begin " body-expr " ) (void))")
      )
    )
  )
)

; Imperative do-while loop. The body is executed at least once.
; Syntax: (do body while condition)
(define-syntax do
  (lambda (tkns)
    (let ((body (parse tkns))
          (while-tkn (pop-tkn tkns))
          (condition (parse tkns))) (begin
      (assert (= while-tkn "while"))
      (strcat
        "(begin " body " (while " condition " " body "))")))))

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

; Short circuiting boolean AND operator
; Syntax: (and arg1 arg2 arg3 ...)
; Takes zero or more arguments. Result is #t if no arguments are given.
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

; Short circuiting boolean OR operator
; Syntax: (or arg1 arg2 arg3 ...)
; Takes zero or more arguments. Result is #f is no arguments are given.
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
