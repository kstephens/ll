; $Id: closed.scm,v 1.3 2007/12/21 03:55:41 stephens Exp $

(load "ll_test.scm")

(define x (cons 1 2))
(define y (cons 3 4))

(define car-closure 
  (add-method ((make <operation>) (<pair> car) self)
	      (lambda () 
		car)))  		

(ll:test:assert
 1	
 ((car-closure x)) 
)

(ll:test:assert
 3
 ((car-closure y)) 
)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define set-cdr-closure 
  (add-method ((make <operation>) (<pair> cdr) self)
	      (lambda (value)	
		(set! cdr value))))
  
((set-cdr-closure x) 'b)
(ll:test:assert
 '(1 . b)
 x)

((set-cdr-closure y) 'd)
(ll:test:assert
 '(3 . d)
 y)

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define car-locative-closure 
  (add-method ((make <operation>) (<pair> car) self)
	      (lambda ()	
		(make-locative car))))
  
(set-contents! ((car-locative-closure x)) 'a)
(ll:test:assert
 '(a . b)
 x)

(set! (contents ((car-locative-closure y))) 'c)
(ll:test:assert
 '(a . b)
 x)
(ll:test:assert
 '(c . d)
 y)

