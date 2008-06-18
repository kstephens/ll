
(add-method ((coercer <list>) (<vector>) self) (vector->list self))
(ll:test:assert equal? ((coercer <list>) '#(1 2 3 4)) '(1 2 3 4))

