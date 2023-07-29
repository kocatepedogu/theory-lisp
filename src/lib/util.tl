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
