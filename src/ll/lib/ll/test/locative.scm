(load "ll_test.scm")

(define x 10)

(ll:test:assert '10 x)

; (set! *debug:compile-emit* 2)
(make-locative x)

(ll:test:assert #t (locative? (make-locative x)))

(define xloc (make-locative x))

(ll:test:assert '10 (contents xloc))

(set-contents! xloc 11)

(ll:test:assert '11 x)

(define x (cons 1 2))
(ll:test:assert '(1 . 2) x)

(define l (make-locative (car x)))
(ll:test:assert #t (locative? l))

(set-contents! l 'a)
(ll:test:assert '(a . 2) x)


