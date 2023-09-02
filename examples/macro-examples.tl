(include "library.tl")

; Example 1
; (), [], and {} are recognized as distinct types of parenthesis.
; They are not aliases for each other.
;
; Output:
;  First 6345
;  Second 7345
;  Third 8345

(define-syntax writeline
  (syntax-rules ()
    ((writeline {x [y z]})
     (display 6 x y z (newline))
    )
    ((writeline [(y z) x])
     (display 7 x y z (newline))
    )
    ((writeline (x {y [z]}))
     (display 8 x y z (newline))
    )
  )
)

(constexpr (println "Example 1"))
(constexpr (writeline {3 [4 5]}))
(constexpr (writeline [(4 5) 3]))
(constexpr (writeline (3 {4 [5]})))

; Example 2
; It is possible to match patterns based on literals.
; Literals are included in the literal list so that they are not
; recognized as identifiers.

(define-syntax literals
  (syntax-rules (0 1 2)
    ((literals 0)
     (+ 2 3))
    ((literals 1)
     (+ 3 4))
    ((literals 2)
     (+ 4 5))))

(constexpr (println "Example 2"))
(constexpr (display (literals 2) (newline)))
(constexpr (display (literals 1) (newline)))
(constexpr (display (literals 0) (newline)))

; Example 3
; Both the invocation syntax and the transcription syntax can have ellipsis.
; x captures 2 and 3, y captures 4 and 5.
; The macro yields the expression
;  (display (list 2 3) (list 4 5))

(define-syntax ellipsis
  (syntax-rules ()
    ((ellipsis {x ...} {y ...})
     (display (list x ...) (list y ...) (newline)))))

(constexpr (println "Example 3"))
(constexpr (ellipsis {2 3} {4 5}))

; Example 4
; An entire expression can be followed by ellipsis in the invocation syntax.
; Then the entire expression is repeatedly matched with remaining expressions in the invocation.
; The macro yields the expression
;  (cons (list 2 3 4 5) (list -1 0 1 2))

(define-syntax example4
  (syntax-rules ()
    ((example4 (H T ...) ...)
     (cons (list H ...) (list T ...)))))

(constexpr (println "Example 4"))
(constexpr (display (example4 (2 -1) (3 0) (4 1) (5 2)) (newline)))

; Example 5
; Consecutive ellipsized expressions are allowed in the transcription syntax, but
; they cannot be used in the invocation syntax as it is ambiguous which expression
; belongs to which.
; The macro yields the expression
;  (list 1 2 3 1 2 3)

(define-syntax twice
  (syntax-rules ()
    ((twice x ...)
     (list x ... x ...))
  )
)

(constexpr (println "Example 5"))
(constexpr (display (twice 1 2 3) (newline)))

; Example 6
; An entire expression can be followed by ellipsis in the transcription syntax.
; Then the expression is repeatedly expanded until all unexpanded variables inside
; expression are fully expanded.
; The macro yields the expression
;  (list (cons 1 2) (cons 1 3) (cons 1 4) (cons 1 5))

(define-syntax example6
  (syntax-rules ()
    (
     (example6 H T ...)
     (list (cons H T) ...)
    )
  )
)

(constexpr (println "Example 6"))
(display (example6 1 2 3 4 5) (newline))

; Example 7
; A more practical use of macros

(define-syntax def
  (syntax-rules (\)
    ((def (func-name) body)
     (define func-name (lambda () body)))
    ((def (func-name args ...) body)
     (define func-name (lambda (args ...) body)))
  )
)

(constexpr (println "Example 7"))

(def (func)
  (println "zero arguments"))
(func)

(def (func x)
 (println (+ x 1)))
(func 2)

