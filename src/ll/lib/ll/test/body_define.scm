(load "ll_test.scm")

(define (f1 x)
  (define (f1f1 a) 
    (define (q a) (write a) (newline))
    (q a)
    (* 3 a)
    )

  (define (f1f2 b) 
    (f1f1 (+ b 2))
    (write b) (newline))

  (f1f1 2)
  (f1f2 3)
  (f1f1 (+ 5 x))
)

(ll:test:assert
 36
 (f1 7)
)

