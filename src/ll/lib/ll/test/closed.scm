; $Id: closed.scm,v 1.3 2007/12/21 03:55:41 stephens Exp $

(load "ll_test.scm")

(define x 'x)
(define y 'y)

(for-each (lambda (x) 
	    (write (cons x y)) (display #\  )) 
	  '(1 2 3 4))

(for-each (lambda (y)
	    (for-each (lambda (x) 
			(write (cons x y)) (display #\  )) 
		      '(1 2 3 4))
	    (newline)) '(a b c d))

(define (foo x . opts)
  ; (%bcc-1) (%bcc-2) (%bcc-emit)
  (write (list 'x x 'opts opts)) (newline)
  (let ((port (if (pair? opts) (car opts) (*current-output-port*))))
    ; (%bcc-1) (%bcc-2)
    (cons x port)))

(ll:test:assert
 (cons 1 (*current-output-port*))
 (foo 1)
)
 
(ll:test:assert
 (cons 1 2)
 (foo 1 2)
)

