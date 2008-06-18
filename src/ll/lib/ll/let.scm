; $Id: let.scm,v 1.1 1999/03/03 07:53:38 stephensk Exp $
 
(define (%let-var binding)
   (car binding))

(define (%let-init binding)
   (car (cdr binding)))

(define (%let-vars bindings)
  (if (null? bindings)
    '()
    (cons (%let-var (car bindings)) (%let-vars (cdr bindings)))))

(define (%let-inits bindings)
  (if (null? bindings)
    '()
    (cons (%let-init (car bindings)) (%let-inits (cdr bindings)))))

(define %let-undef '#u)

(define (%let-undefs bindings)
  (if (null? bindings)
    '()
    (cons %let-undef (%let-undefs (cdr bindings)))))

(define (%let-sets bindings)
  (if (null? bindings)
    '()
    (cons (list 'set! (%let-var (car bindings)) (%let-init (car bindings))) (%let-sets (cdr bindings)))))

(define-macro (let bindings . body)
  (cons (cons 'lambda (cons (%let-vars bindings) body)) (%let-inits bindings)))

(define (%let* bindings body)
  (if (null? bindings)
     body
     (list (cons 'lambda (list (%let-var (car bindings))) (%let* (cdr bindings))) (%let-init (car bindings)))))

(define-macro (let* bindings . body)
  (%let* bindings body))

(define-macro (letrec bindings . body)
  (cons (cons 'lambda (cons (%let-vars bindings) (cons %let-sets bindings) body)) (%let-undefs bindings)))


