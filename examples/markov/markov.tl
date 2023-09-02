(include "automata.tl")
(include "util.tl")

; Total number of steps
(define STEPS 1000)

; Number of visits for each state
(define A 0)
(define B 0)

; Stop condition
(define done?
  {= STEPS + A B})

; Definiton of the automaton
(define MarkovChain
  (automaton\0
    (qA:P {(set! A (+ A 1))}
      (done? accept)
      ({p 0.3} qA)
      ({p 0.7} qB))
    (qB:P {(set! B (+ B 1))}
      (done? reject)
      ({p 0.2} qB)
      ({p 0.8} qA))))

; Run automaton
(let ((result (MarkovChain)))
  (println "Decision: " (get-decision result)))

; Print results
(let ((pA (* 100.0 (/ A STEPS)))
      (pB (* 100.0 (/ B STEPS))))
  (println "Visit Percentages: qA: " pA " qB: " pB))
