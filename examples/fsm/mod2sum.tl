(include "util.tl")
(include "automata.tl")

(define mod2sum
  (automaton\1
    (q0:R {(set! result 0)}
      ({= 0} . q0)
      ({= 1} . q1)
      ({= blank} . halt)
    )

    (q1:R {(set! result 1)}
      ({= 0} . q1)
      ({= 1} . q0)
      ({= blank} . halt)
    )
  )
)

(let ((result -1))
  (begin
    (mod2sum (make-tape 1 1 0 1))
    (println "Result:" result)
  )
)

(exit)
