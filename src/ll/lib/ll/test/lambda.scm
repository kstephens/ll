;; basic lambda
(lambda (a b c) a)

(load "ll_test.scm")

;; basic lambda
(define (x a b c)  
  b)

(ll:test:assert '2 (x 1 2 3))

;; car pos, locals.
(let ((b (lambda (a b) (* 2 a b)))
      (a (lambda (b a) (* 3 a b)))
      )
  (ll:test:assert '180 (b (a 2 3) 5))
  )

;; car pos
(ll:test:assert
 '(3 5 3 5)
 ((lambda (a b) (list a b a b)) (+ 1 2) 5)
)