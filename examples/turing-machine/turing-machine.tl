(include "automata.tl")

(define sep "#")

(define M1
  (automaton\1
    (q1:(W sep))
    (q2:blankOnR)
    (q3:(W sep))
    (q4:(onL sep))
    (q5:R
      ({= sep} . halt)
      ({= "d"} . next)
      ({else} . self))
    (q6:(W blank))
    (q7:(onR sep))
    (q8:blankOnR)
    (q9:(W "d"))
    (q10:(onL sep)
      ({#t} . q4))
  )
)

(define M2
  (automaton\1
    (q1:(nonR "d"))
    (q2:L
      ({= "d"} . q3)
      ({= sep} . halt))
    (q3:(W "c"))
    (q4:(onL sep))
    (q5:L
      ({= sep} . reject)
      ({= "c"} . next)
      ({else} . self))
    (q6:(W blank))
    (q7:(onR sep)
      ({#t} . q1))
  )
)

(define M3
  (automaton\1
    (q1:(onL sep))
    (q2:R
      ({= "c"} . next)
      ({= sep} . halt)
      ({else} . self))
    (q3:(W blank))
    (q4:(onR sep))
    (q5:blankOnR)
    (q6:(W "c"))
    (q7:(onL sep)
      ({#t} . q1))
  )
)

(define M4
  (automaton\1
    (q1:(nonR "c"))
    (q2:L
      ({= "c"} . q3)
      ({= sep} . accept))
    (q3:(W "b"))
    (q4:(onL sep))
    (q5:L
      ({= sep} . reject)
      ({= "b"} . q6)
      ({else} . self))
    (q6:(W blank))
    (q7:(onR sep)
      ({#t} . q1))
  )
)

(define comparator
  (automaton\1
    (q1:M1)
    (q2:M2)
    (q3:M3)
    (q4:M4)
  )
)

(define result
 (comparator (make-tape "a" "d" "c" "b" "a")))

(define decision (get-decision result))
(display "Decision: " (get-decision result) nl)

(define tape (at 0 (get-tapes result)))
(define head (get-head tape))
(display "Head position: " head nl)
(define contents (get-contents tape))
(display "Tape contents: " (tape-tostring contents) nl)

