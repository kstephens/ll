; $Id: mycons.scm,v 1.4 2008/01/01 10:19:17 stephens Exp $
(load "ll_test.scm")

(define <my-cons> (make <type> (list <object>) '(car cdr)))

(add-method (initialize (<my-cons> car cdr) self x y)
  ; (%bc:debug)
  (set! car x)
  (set! cdr y)
  self)

(define my-car (make <settable-operation>))
(add-method (my-car (<my-cons> car) self)
   ; (%bc:debug)
   car)
(add-method ((setter my-car) (<my-cons> car) self value)
  (set! car value))

(define my-cdr (make <settable-operation>))
(add-method (my-cdr (<my-cons> cdr) self)
   cdr)
(add-method ((setter my-cdr) (<my-cons> cdr) self value)
  (set! cdr value))

(define x (make <my-cons> 1 2))
(ll:test:assert 1 (my-car x))
(ll:test:assert 2 (my-cdr x))

(set! (my-car x) 3)
((setter my-cdr) x 4)

(ll:test:assert 3 (my-car x))
(ll:test:assert 4 (my-cdr x))

