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

; Computes a value during parsing (before runtime)
(define-syntax constexpr
  (lambda (tkns)
    (strcat
      (eval (strcat (parse tkns))))))

; Defines a variable available during parsing (before runtime)
(define-syntax define-constexpr
  (lambda (tkns)
    (let ((name (pop-tkn tkns)))
      (let ((define-expr (strcat "(define " name " " (parse tkns) " )")))
        (begin (eval define-expr) "(void)")))))

(define-constexpr remove-quotes
  (lambda (str)
    (if (= str "") ""
      (if (= (strcar str) (quotation-mark))
        (strcat " " (remove-quotes (strcdr str)))
        (strcat (strcar str) (remove-quotes (strcdr str)))))))

(define-syntax assert
  (lambda (tkns)
    (let ((con (parse tkns)))
      (strcat
        "(if (not " con ")
           (error (strcat "
             (quotation-mark)
               "Assertion failure: " (remove-quotes con)
             (quotation-mark) "))
           (void))"))))

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

; Syntax: (do body while condition)
(define-syntax do
  (lambda (tkns)
    (let ((body (parse tkns))
          (while-tkn (pop-tkn tkns))
          (condition (parse tkns))) (begin
      (assert (= while-tkn "while"))
      (strcat
        "(begin " body " (while " condition " " body "))")))))

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

(define-constexpr list-contains
  (lambda (lst key)
    (if (null? lst)
      #f
      (if (= (car lst) key)
        #t
        (list-contains (cdr lst) key)))))

(define-constexpr map
  (lambda (func lst)
    (if (null? lst)
      null
      (list (func (car lst)) %(map func (cdr lst)))
    )
  )
)

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

(define-constexpr next-structured-expr
  (lambda (peek-func pop-func)
    ; Initially the stack contains the left-most parenthesis
    ; and the result is empty
    (let ((stack (list (peek-func)))
          (result (list (pop-func)))) (begin
      ; While stack is non-empty, push opposites of '(', '[', '{' to the stack
      ; and match them with ')', ']', '}' respectively. Meanwhile,
      ; all tokens are appended to the result.
      (while (not (null? stack))
        (let ((new-token (pop-func))) (begin
          (cond
            ((= new-token "(") (push! stack "("))
            ((= new-token "[") (push! stack "["))
            ((= new-token "{") (push! stack "{"))
            ((= new-token ")") (assert (= (pop! stack) "(")))
            ((= new-token "]") (assert (= (pop! stack) "[")))
            ((= new-token "}") (assert (= (pop! stack) "{"))))
          (set! result (list %result new-token)))))
      ; Return the result
      result))))

(define-constexpr expr-list-to-string
  (lambda (lst)
    (if (null? lst)
      ""
      (strcat (car lst) " " (expr-list-to-string (cdr lst))))))

(define-constexpr token-list-replace-with-matches
  (lambda (lst matches) (begin
    (if (null? lst)
      null
      (cons
        (let ((value (dict-get matches (car lst))))
          (if (void? value)
            (car lst)
            (expr-list-to-string value)))
        (token-list-replace-with-matches (cdr lst) matches))))))

(define-constexpr is-open-parenthesis
  (lambda (p)
    (or (= p "(") (= p "[") (= p "{"))))


(define-constexpr is-close-parenthesis
  (lambda (p)
    (or (= p ")") (= p "]") (= p "}"))))

(define-constexpr get-closing-parenthesis
  (lambda (p)
    (cond ((= p "(") ")") ((= p "[") "]") ((= p "{") "}"))))

(define-constexpr substitute-expression
  (lambda (assignments unexpanded-assignments transcription-syntax)
    (if (null? transcription-syntax)
      ; An empty transcription syntax is base case in recursion.
      (cons null null)
      ; If transcription syntax is non-empty, do substitutions recursively.
      (let ((tkn-in-syntax (pop! transcription-syntax)))
        (if (is-open-parenthesis tkn-in-syntax)
          ; Parenthesized expressions
          (let ((closing-parenthesis (get-closing-parenthesis tkn-in-syntax))
                (unexpanded-vars (list))
                (result (list tkn-in-syntax))
                (ellipsis #f)
                (new-transcription-syntax transcription-syntax)) (begin
            ; Check if there is an ellipsis after the expression
            (set! ellipsis #f)
            (let ((temp-syntax (cons tkn-in-syntax transcription-syntax)))
              (let ((temp-expr (next-structured-expr {car temp-syntax} {(pop! temp-syntax)})))
                (when (not (null? temp-syntax))
                  (set! ellipsis (= (car temp-syntax) "...")))))

            ; Until the closing parenthesis is found, substitute expressions
            (while (!= (car new-transcription-syntax) closing-parenthesis)
              (let ((sub-result (substitute-expression assignments (if ellipsis null unexpanded-assignments) new-transcription-syntax))) (begin
                (set-putall! unexpanded-vars %(car sub-result))
                (set! result (list %result %(car (cdr sub-result))))
                (set! new-transcription-syntax (car (cdr (cdr sub-result)))))))
            ; Add the closing parenthesis to the result
            (set! result (list %result closing-parenthesis))
            (pop! new-transcription-syntax)

            ; If an ellipsis follows the parenthesized expression
            (when ellipsis
              (let ((local-unexpanded-assignments (list))) (begin
                ; Initialize local unexpanded assignments
                (map {(dict-put! local-unexpanded-assignments ($1 (dict-get assignments $1)))} unexpanded-vars)

                ; Until no unexpanded variables left, repeatedly substitute the expression
                (while (not (null? local-unexpanded-assignments)) (begin
                  ; Each time remove the first expression of every unexpanded variable (it is previously processed)
                  (set! local-unexpanded-assignments
                    (let ((loop (lambda (dict)
                                  (if (null? dict)
                                    null
                                    (let ((pair (car dict)))
                                      (let ((key (car pair)) (value (cdr pair)))
                                        (if (null? (cdr value))
                                          (loop (cdr dict))
                                          (cons (cons key (cdr value)) (loop (cdr dict))))))))))
                      (loop local-unexpanded-assignments)))

                  ; If there are is waiting expansion
                  (when (not (null? local-unexpanded-assignments))
                    ; In the next iteration start from the beginning of the parenthesized expression
                    (set! new-transcription-syntax transcription-syntax)
                    ; Insert the opening parenthesis to the result
                    (set! result (list %result tkn-in-syntax))

                    ; Until the closing parenthesis is found, substitute expressions
                    (set! unexpanded-vars (list))
                    (while (and (not (null? new-transcription-syntax)) (!= (car new-transcription-syntax) closing-parenthesis)) (begin
                      (let ((sub-result (substitute-expression assignments local-unexpanded-assignments new-transcription-syntax))) (begin
                        (set-putall! unexpanded-vars %(car sub-result))
                        (set! result (list %result %(car (cdr sub-result))))
                        (set! new-transcription-syntax (car (cdr (cdr sub-result))))))))

                    ; After the end of the expression add the closing parenthesis to the result
                    (set! result (list %result closing-parenthesis))
                    ; Remove the closing parenthesis from the transcription syntax
                    (pop! new-transcription-syntax))))))

                ; Remove the ellipsis from the transcription syntax
                (pop! new-transcription-syntax))

            ; Return constructed result
            (list unexpanded-vars result new-transcription-syntax))
          )

          ; Identifiers and literals.
          (if (void? (dict-get assignments tkn-in-syntax))
            ; If the token does not have a corresponding value in the assignment dictionary, leave it as is.
            (list (list) (list tkn-in-syntax) transcription-syntax)
            ; Otherwise, replace the token.
            (let ((ellipsis-found #f)) (begin
              ; Check if an ellipsis directly follows the pattern variable
              (when (not (null? transcription-syntax))
                (set! ellipsis-found (= (car transcription-syntax) "...")))

              ; If an ellipsis exists
              (if ellipsis-found (begin
                  ; Remove the ellipsis
                  (set! transcription-syntax (cdr transcription-syntax))
                  ; Fully expand the pattern variable
                  (list (list)
                        (list (expr-list-to-string (dict-get assignments tkn-in-syntax)))
                        transcription-syntax))

              ; If an ellipsis does not exist
              (if (null? (cdr (dict-get assignments tkn-in-syntax)))
                ; If the pattern variable contains a single expression, expand it.
                (list (list)
                      (list (car (dict-get assignments tkn-in-syntax)))
                      transcription-syntax)

                ; If the pattern variable contains multiple expressions
                (if (void? (dict-get unexpanded-assignments tkn-in-syntax))
                  ; If the "further expansion" of the pattern variable has not yet started,
                  ; expand the first expression in the variable, but add the variable to the
                  ; list of variables that require further expansion.
                  (list (list tkn-in-syntax)
                        (list (car (dict-get assignments tkn-in-syntax)))
                        transcription-syntax)

                  ; If the "further expansion" has already started, expand the first expression
                  ; that is given for the variable in the unexpanded-assignments dictionary.
                  (let ((assignment (dict-get unexpanded-assignments tkn-in-syntax)))
                    (if (null? (cdr assignment))
                      ; If this was the final expression in the further expansion.
                      (list (list)
                            (list (car assignment))
                            transcription-syntax)
                      ; If this was not the final expression in the further expansion.
                      (list (list tkn-in-syntax)
                            (list (car assignment))
                            transcription-syntax))))))))
          )
        )
      )
    )
  )
)

(define-constexpr match-expression
  (lambda (stream invocation-syntax literal-list) (begin
    (let ((first-token-in-syntax (pop! invocation-syntax))
          (first-token-in-args (pop! stream))) (begin
      (assert (= first-token-in-syntax first-token-in-args))
      (cond
        ((= first-token-in-syntax "(") (match-parenthesis stream invocation-syntax ")" literal-list))
        ((= first-token-in-syntax "[") (match-parenthesis stream invocation-syntax "]" literal-list))
        ((= first-token-in-syntax "{") (match-parenthesis stream invocation-syntax "}" literal-list))
        (#t (error "A matching expression must begin with one of '(', '[', or '{'"))))))))

(define-syntax pattern-append!
  (lambda (tkns)
    (let ((dict-name (pop-tkn tkns))
          (key (parse tkns))
          (value (parse tkns)))
      (strcat "
        (let ((existing-value (dict-get "dict-name" "key")))
          (if (void? existing-value)
            (dict-put! "dict-name" ("key" (list "value")))
            (dict-put! "dict-name" ("key" (list %existing-value "value"))))) "))))

(define-syntax pattern-appendall!
  (lambda (tkns)
    (let ((dict-name (pop-tkn tkns))
          (value (parse tkns)))
      (strcat "
        (let ((lst " value "))
          (while (not (null? lst))
            (let ((key (car (car lst)))
                  (value (cdr (car lst))))
              (begin
                (pattern-append! " dict-name " key (expr-list-to-string value))
                (set! lst (cdr lst)))))) "))))

(define-constexpr match-parenthesis
  (lambda (stream invocation-syntax closing-parenthesis literal-list)
    ; Create an empty dictionary of variable assignments
    (let ((dict (list))) (begin
      ; Until the closing parenthesis is found, match identifiers, literals and expressions
      (while (!= (car invocation-syntax) closing-parenthesis)
        (let ((tkn-in-syntax (car invocation-syntax)))
          (cond
            ; If the syntax begins with an opening parenthesis, match it recursively.
            ; Example: Syntax=(a b c), Stream=(arg1 arg2 arg3)
            ((is-open-parenthesis tkn-in-syntax)
              ; Recursively match
              (let ((sub-match (match-expression stream invocation-syntax literal-list))) (begin
                ; Include new assignments in the dictionary
                (dict-putall! dict (cdr (cdr sub-match)))
                ; Get the remaining part of the input stream.
                (set! stream (car sub-match))
                ; Get the remaining part of the invocation syntax.
                (let ((new-invocation-syntax (car (cdr sub-match)))) (begin
                  ; If there is an ellipsis on the invocation syntax, continue matching expressions from the input.
                  (if (= (car new-invocation-syntax) "...") (begin
                    (while (!= (car stream) (get-closing-parenthesis tkn-in-syntax))
                      (let ((new-sub-match (match-expression stream invocation-syntax literal-list))) (begin
                        (pattern-appendall! dict (cdr (cdr new-sub-match)))
                        (set! stream (car new-sub-match))))
                    )

                    ; Remove ellipsis
                    (pop! new-invocation-syntax)) (void)
                  )

                  ; Update invocation syntax
                  (set! invocation-syntax new-invocation-syntax))))))


            ; If the syntax begins with a literal, check if it matches.
            ; Example: Syntax=else, Stream=else (assuming else is in the literal list)
            ; Example: Syntax=0, Stream=0 (assuming 0 is in the literal list)
            ((list-contains literal-list tkn-in-syntax) (begin
              (pop! invocation-syntax)
              (let ((new-token (pop! stream)))
                (assert (= tkn-in-syntax new-token)))))

            ; If the syntax begins with an identifier, assign it to the expression that appear on the stream.
            ; Example: Syntax=x, Stream=some-identifier
            ; Example: Syntax=x, Stream=(+ 2 3)
            ; Example: Syntax=x, Stream="2"
            (#t (begin
              (pop! invocation-syntax)
              (do
                ; If the peek token on the stream is an opening parenthesis, it is an entire structured expression.
                (if (is-open-parenthesis (car stream)) (begin
                  (let ((new-expression
                        (expr-list-to-string (next-structured-expr {car stream} {(pop! stream)}))))
                    (pattern-append! dict tkn-in-syntax new-expression)))
                ; Otherwise the expression on the stream has a single token.
                (let ((new-token (pop! stream)))
                  (pattern-append! dict tkn-in-syntax new-token)))
              ; If the identifier is followed by an ellipsis, continue matching until the closing parenthesis.
              while (and (= (car invocation-syntax) "...") (!= (car stream) closing-parenthesis)))

              ; If the identifier is variadic, remove ellipsis token.
              (if (= (car invocation-syntax) "...")
                (pop! invocation-syntax) (void))))
            )
          )
        )

      ; Remove the closing right parenthesis
      (pop! invocation-syntax)
      (pop! stream)
      ; Return the remaining part of the invocation syntax
      (cons stream (cons invocation-syntax dict))))))

(define-syntax syntax-rules
  (lambda (tkns)
    (let ((syntax-list (list))
          (literal-list (list))
          (done #f))
      (begin
        ; Get literals from the literal list
        (assert (= "(" (pop-tkn tkns)))
        (while (!= (peek-tkn tkns) ")")
          (push! literal-list (pop-tkn tkns)))
        (assert (= ")" (pop-tkn tkns)))
        ; Store invocation-transcription pairs in syntax-list in the order they appear.
        (while (not done)
          (try (begin
                 (assert (= "(" (pop-tkn tkns)))
                 (let ((invocation-syntax (next-structured-expr {peek-tkn tkns} {pop-tkn tkns}))
                       (transcription-syntax
                         (if (or (= (peek-tkn tkns) "(") (= (peek-tkn tkns) "[") (= (peek-tkn tkns) "{"))
                           ; Transcription syntax can be either a structured expression
                           (next-structured-expr {peek-tkn tkns} {pop-tkn tkns})
                           ; or a single token (literal data expression or identifier expression)
                           (list (pop-tkn tkns)))))
                   (set! syntax-list (list %syntax-list (cons invocation-syntax transcription-syntax))))
                 (assert (= ")" (pop-tkn tkns)))
               )
            (catch (e) (begin
              (set! done #t)
              ; Return to the token immediately before the one that caused an assertion failure.
              (prev-tkn tkns))
            )
          )
        )

        ; Generate a procedure that does the pattern matching
        (strcat
          "(lambda (tkns) (begin
            ; Go to the beginning of the macro invocation on the token stream
            (prev-tkn tkns)
            (prev-tkn tkns)

            ; Get input and try matching it with each invocation syntax.
            (let ((failures (strcat))
                  (result (void))
                  (syntax-list " syntax-list ")
                  (literal-list " literal-list ")
                  (input (next-structured-expr {peek-tkn tkns} {pop-tkn tkns})))
             (begin
               (while (and (not (null? syntax-list)) (void? result)) (begin
                 (set! syntax-pair (car syntax-list))
                 (set! invocation-syntax (car syntax-pair))
                 (set! transcription-syntax (cdr syntax-pair))

                 (try (begin
                   ; Try to match the input and the invocation syntax
                   (let ((match-result (match-expression input invocation-syntax literal-list)))
                     (let  ((remaining-input (car match-result))
                            (remaining-syntax (car (cdr match-result)))
                            (matches (cdr (cdr match-result)))) (begin

                       ; Entire input must be consumed for a valid match.
                       (assert (null? remaining-input))

                       ; If the match is valid, construct result using transcription syntax
                       (set! result (expr-list-to-string (car (cdr (substitute-expression matches (list) transcription-syntax)))))))))
                   (catch (e)
                     (strcat! failures (newline) e)))

                 ; Try the next match
                 (set! syntax-list (cdr syntax-list))))

               ; Go back onto the right parenthesis before returning
               ; control to the internal parser
               (prev-tkn tkns)

               ; Return transcription syntax with substitutions
               (if (not (void? result))
                 result
                 (error (strcat" (quotation-mark) "Pattern matching failed." (quotation-mark) "failures)))))))"
        )
      )
    )
  )
)

