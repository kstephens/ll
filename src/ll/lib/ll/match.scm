;;;; $Id: match.scm,v 1.1 2001/01/15 04:03:27 stephens Exp $

(define (match-binding-var b) (car b))
(define (match-binding-value b) (cdr b))
(define (make-match-binding var val) (cons var val))
(define make-match-binding cons)

(define (find-match-binding bs var)
  (if (null? bs)
      #f                                                     ; no binding found
      (if (eq? (match-binding-var (car bs)) var)
	  (car bs)                                           ; found binding
	  (find-match-binding (cdr bs) var))))               ; continue

(define (match e p bs)
  (if (pair? p)
      (if (eq? (car p) '?)                                   ; pattern is (? <v>)
	  (let* ((v (cadr p))
		 (b (find-match-binding bs v)))              ; lookup existing match
	    (if b
		(if (equal? e (match-binding-value b))
		    bs                                       ; existing matched expr
		    #f)                                      ; existing did not match expr

		;; No binding found yet
		(cons (make-match-binding v e) bs)))         ; Return new binding
	  ;; pattern must not be a (?) but a literal pair.
	  ;; expr must also be a pair

	  (if (pair? e)
	      (begin
		(set! bs (match (car e) (car p) bs))
		(if bs
		    (match (cdr e) (cdr p) bs)
		    bs))

	      ;; expr is not a pair, but pattern is => no match
	      #f))
      ;; pattern is not a pair, e and p must be matching literals
      (if (equal? e p)
	  bs
	  #f)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(match 'x 'y '())
(match 'x 'x '())
(match '(a b) '(a b) '())

(match 1 '(? x) '())
(match '(a b c d) '(a (? x) . (? y)) '())

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define unify:error '(unify:error))

(define (unify e bs)
  (if (pair? e)
      (if (eq? (car e) '?)
	  (let* ((v (cadr e))
		 (b (find-match-binding bs v)))
	    (if b
		(match-binding-value b)
		unify:error))
	  (cons (unify (car e) bs) (unify (cdr e) bs)))
      e))

	  
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
