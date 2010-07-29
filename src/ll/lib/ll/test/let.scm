(load "ll_test.scm")

(let ((x #f))
  (ll:test:assert #f x)
  (set! x #t)
  (ll:test:assert #t x)
)

(newline)'ok
