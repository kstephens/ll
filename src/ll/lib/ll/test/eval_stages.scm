(define *stdout* (*current-output-port*))
(define (d x)
  (newline) (display x) (newline)
)
(define (p x)
  (newline) (write x) (newline)
)
(define (pi x)
  (p x)
  (%write-ir x *stdout*) (newline)
)

(define x (cons 1 2))
; (p x)

(d ";  The expression ==>")
(define x0 '(lambda (y . args) (set! (car x) y)))
(p x0)

(d ";  eval-stage-1 ==>")
(define x1 (eval-stage-1 x0))
(p x1)

(d ";  eval-stage-2 ==>")
(define x2 (eval-stage-2 x1))
(pi x2)
(pi (cadar (%ir-body x2)))

(if #t
  (begin
    (d ";  eval-stage-3 ==>")
    (define x3 (eval-stage-3 x2))
    (pi x3)
    (pi (cadar (%ir-body x3)))
  )
  (begin
    (define x3 x2)
  )
)

(d ";  eval-stage-4 ==>")
(define x4 (eval-stage-4 x3))
(pi x4)
(pi (cadar (%ir-body x4)))

