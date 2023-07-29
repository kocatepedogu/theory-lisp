(include "util.tl")

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
            (else (list &(car va_args) &(append &(cdr va_args))))
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
      (list (func (car lst)) &(map func (cdr lst)))
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
