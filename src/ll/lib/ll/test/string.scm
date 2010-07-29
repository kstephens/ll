(load "ll_test.scm")

(let ((x #f) (y #f))

; (%bc:debug)

(newline)(write 'equal?)(newline)
(set! x (string #\a #\b))
(set! y (string #\a #\b))
(ll:test:assert #f (eq? x y))
(ll:test:assert #f (eqv? x y))
(ll:test:assert #t (equal? x y))

(newline)(write 'clone)(newline)
(set! x (string #\a #\b))
(ll:test:assert #t (eq? x x))
(ll:test:assert #f (eq? (clone x) x))

)

(newline)'ok
