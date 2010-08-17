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

;; rest args
(define (x a . r)
  r)
(ll:test:assert
 '(2)
 (x 1 2)
)

;; closed-over
(define (y a) (a 1))
(define (x)
  (%bc:debug)
  (let ((closed-over (if #f 2 3)))
    (y (lambda (b) closed-over))))
(ll:test:assert
  3
  (x))

'ok

