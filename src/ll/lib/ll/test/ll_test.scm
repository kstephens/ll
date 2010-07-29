(define (ll:test:assert x y . opt)
  (let ((cmp (if (pair? opt) (car opt) equal?)))
    (if (cmp x y)
	(begin
	  (display '.)
	  #t
	  )
	(ll:test:error "expected " x " found " y)))
  )

(define (ll:test:error . args)
  (display 'E)
  (newline)
  (display "ll:test:error: ")
  (display args) 
  (newline)
  (raise <error> 'test-failed args)
)

