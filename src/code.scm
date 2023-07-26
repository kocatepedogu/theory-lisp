;; (list 'abc 1)
;; (list 1 2 3)
;; (define a 3)
;; (car (cons 2 null))
;; (cdr (cons 2 null))
;;
;; (define func1
;;   (lambda (x y) (+ x y)))
;;
;; (define func2
;;   (lambda (x y) (* x y)))
;;
;; (define result (func2 (func1 3 5) (func1 2 4)))
;;    (+ 2 3)
;;    (cons 1 2)
;;
;;   (define - (lambda (x y) (* x y)))
;;   (define apply (lambda (f x y) (f x y)))
;;   (apply - 3 2)

  ;(define variadic-function (lambda (x y ...) (list y x va_args)))
  ;(variadic-function 1 2 3 4)

  (define head (lambda (x ...) x))
  (define tail (lambda (x ...) va_args))
  (head 1 2 3)
  (tail 1 2 3)

  (define values (list 1 2 3))
  (+ &values)

  (define forward (lambda (...) (* &va_args)))
  (forward 1 2 3 4 5)

;(list 1 2 3 (list 10 20) () void)
;(list () () (list () ()))

'lambda_test
(define func1
  (lambda (x y) (- x y)))

(define func2
  (lambda (x y z) (+ x (func1 y z))))

(func2 10 50 30)

'arithmetic_test
(/ (* 2 (+ 3 (- 10 5))) 4)
;;
;; 'type_test
;; (procedure? 3)
;; (procedure? func2)
;; (number? 3)
;; (number? 4.0)
;; (number? 'x)
;;
;; 'and_test
;; (and #t #f)
;; (and #f #t)
;; (and #t #f)
;; (and #t #t)
;;
;; 'or_test
;; (or #f #f)
;; (or #f #t)
;; (or #t #f)
;; (or #t #t)
;;
;; 'xor_test
;; (xor #f #f)
;; (xor #f #t)
;; (xor #t #f)
;; (xor #t #t)
;;
;; 'not_test
;; (not #f)
;; (not #t)
;;
;; 'comparison_test
;; (> 5.0 4.0)
;; (> 5.0 5.0)
;; (>= 5.0 4.0)
;; (>= 5.0 5.0)
;; (< 5.0 4.0)
;; (< 5.0 5.0)
;; (<= 5.0 4.0)
;; (<= 5.0 5.0)

;'list_test
;; (define lst
;;   (cons 'a (cons 'b (cons 'c (cons 'd null)))))
;;
;; (define head1 (car lst))
;; (define tail1 (cdr lst))
;;
;; (define head2 (car tail1))
;; (define tail2 (cdr tail1))
;;
;; (define head3 (car tail2))
;; (define tail3 (cdr tail2))
;;
;; (define head4 (car tail3))
;; (define tail4 (cdr tail3))
;;
;; (define head5 (car tail4))
;; (define tail5 (cdr tail4))
;;
;; (list 1 2 3)
