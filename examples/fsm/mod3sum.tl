(include "util.tl")
(include "primitives.tl")
(include "string.tl")

(define mod3sum
  (automaton\1
    (q0:R {(set! result 0)}
      ({= 0} . q0)
      ({= 1} . q1)
      ({= 2} . q2)
      ({= blank} . halt)
    )
    (q1:R {(set! result 1)}
      ({= 0} . q1)
      ({= 1} . q2)
      ({= 2} . q0)
      ({= blank} . halt)
    )
    (q2:R {(set! result 2)}
      ({= 0} . q2)
      ({= 1} . q0)
      ({= 2} . q1)
      ({= blank} . halt)
    )
  )
)

(define compute-mod3sum
  (lambda (...)
    (let ((result -1))
      (begin
        (mod3sum (make-tape &va_args))
        result)
    )
  )
)

(display (compute-mod3sum 0 1 2 1 2 2 1 2 1 1))
