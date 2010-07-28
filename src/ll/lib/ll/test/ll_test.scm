(define (ll:test:assert x y . opt)
  (display '.)
  (let ((cmp (if (pair? opt) (car opt) equal?)))
    (if (cmp x y)
	#t
	(ll:test:error "expected " x " found " y)))
  )

(define (ll:test:error . args)
  (newline)
  (display "ll:test:error: ")
  (display args) 
  (newline)
  (raise <error> 'test-failed args)
)

