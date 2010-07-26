(define (prime-factors n)
  (prime-factors-1 n (primes-less-than (/ n 2)) '()))

(define (prime-factors-1 n primes factors)
  (if (null? primes)
      factors
      (let ((f (car primes)))
	(if (divisible-by? n f)
	    (cons f (prime-factors-1 (/ n f) primes factors))
	    (prime-factors-1 n (cdr primes) factors)))))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (primes-less-than n)
  (if (< (car *primes*) n)
      (begin
	(set! *primes* (next-primes *primes*))
	(primes-less-than n))
      (cdr *primes*)))

(define *primes* (list 2))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (next-primes primes)
    (next-primes-1 primes (+ (car primes) 1)))

(define (next-primes-1 primes n)
  (if (any? (lambda (f) (divisible-by? n f)) primes)
      (next-primes-1 primes (+ n 1))
      (cons n primes)))

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (divisible-by? n m)
  (zero? (modulo n m)))

(define (any? f seq)
  (if (null? seq)
      #f
      (if (f (car seq))
	  #t
	  (any? f (cdr seq)))))
      

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


(write (primes-less-than 2))(newline)
(write (primes-less-than 3))(newline)
(write (primes-less-than 4))(newline)
(write (primes-less-than 5))(newline)
(write (primes-less-than 10))(newline)
(write (primes-less-than 100))(newline)
(write (primes-less-than 1000))(newline)
(write (primes-less-than 10000))(newline)

(write (prime-factors 2))(newline)
(write (prime-factors 3))(newline)
(write (prime-factors 5))(newline)
(write (prime-factors 10))(newline)
(write (prime-factors 20))(newline)
(write (prime-factors 2310))(newline)
