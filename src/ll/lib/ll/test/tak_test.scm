;;; takeuchi benchmark in Scheme
;;; see <url:http://www.lib.uchicago.edu/keith/crisis/benchmarks/tak/>
;;; Keith Waclena <k-waclena@uchicago.edu>

(define (tak x y z)
  (if (not (< y x))
      z
      (tak (tak (- x 1) y z)
           (tak (- y 1) z x)
           (tak (- z 1) x y))))

(tak 18 12 6)
(tak 30 15 9)
(tak 33 15 9)
(tak 40 15 9)
