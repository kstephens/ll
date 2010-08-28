(load "ll_test.scm")

(write "basic lambda - 0 args")(newline)
(define a0 (lambda () 0))
(ll:test:assert '0 (a0))

(write "basic lambda - 1 arg")(newline)
(define a1 (lambda (a) a))
(ll:test:assert '1 (a1 1))

(write "basic lambda - 2 args")(newline)
(define a2 (lambda (a b) (cons a b)))
(ll:test:assert '(1 . 2) (a2 1 2))

(write "basic lambda - 3 args")(newline)
(define x (lambda (a b c) a))
(ll:test:assert '1 (x 1 2 3))

(write "basic lambda - 3 args, (define (...) ...)")(newline)
(define (x a b c) b)
(ll:test:assert '2 (x 1 2 3))

(write "rest args - ignored")(newline)
(define a_ (lambda a 123))
(ll:test:assert '123 (a_ 1 2 3))

(write "rest args")(newline)
(define ar (lambda a a))
(ll:test:assert '() (ar))
(ll:test:assert '(1) (ar 1))
(ll:test:assert '(1 2) (ar 1 2))
(ll:test:assert '(1 2 3) (ar 1 2 3))

(write "car pos, locals.")(newline)
(let ((b (lambda (a b) (* 2 a b)))
      (a (lambda (b a) (* 3 a b)))
      )
  (ll:test:assert '180 (b (a 2 3) 5))
  )

(write "car pos")(newline)
(ll:test:assert
 '(3 5 3 5)
 ((lambda (a b) (list a b a b)) (+ 1 2) 5)
)

(write "rest args")(newline)
(define (x a . r)
  r)
(ll:test:assert
 '(2)
 (x 1 2)
)

(write "closed-over")(newline)
(define (y a) (a 1))
(define (x)
  ;; (%bc:debug)
  (let ((closed-over (if #f 2 3)))
    (y (lambda (b) 
	 ;; (%bc:debug)
	 closed-over))))
(ll:test:assert
  3
  (x))

'ok

