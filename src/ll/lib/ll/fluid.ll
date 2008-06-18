; $Id: fluid.ll,v 1.1 1999/03/03 07:53:38 stephensk Exp $

(define-macro (let-fluid* bindings . body)
  (let* ((fsave (%gensym)) (result (%gensym)))
    `(let* ((,fsave %fluid-bindings) (,result #f))
      ,@(map (lambda (x) (%fluid-bind (list quote (car (cdr x))))))
      (set! ,result (begin ,@body))
      (set! %fluid-bindings ,fsave)
      ,result
    )
  )
)
