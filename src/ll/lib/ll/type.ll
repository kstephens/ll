;; Additions to <type>

(define type-ancestors (make <operation>))

(add-method
  (type-ancestors (<type> supers) self . result)
  ;; Note: this is a test of closing over of rest arg.
  ;; The local variable for the 'result rest-arg list is
  ;; initialized, then placed in the export vector.
  ;;
  ;; (%bc:debug)
  ;; (display "ancestors ") (write self) (display " => ") (write '...) (newline)
  (if (null? result)
      (reverse (type-ancestors self '()))
      (begin
	(set! result (car result))
	;; (display "  list ") (write self) (display " => ") (write result) (newline)
	(if (not (memq self result))
	    (begin
	      (set! result (cons self result))
	      ;; (display "  adding ") (write self) (display " => ") (write result) (newline)
	      (for-each (lambda (super)
			  ;; (display "  scanning ") (write super) (newline)
			  (set! result (type-ancestors super result))
			  ;; (display "  scanning result ") (write super) (display " => ") (write result) (newline)
			  )
			supers
			)
	      )
	    )
	;; (display "result ") (write self) (display " => ") (write result) (newline)
	result
	)
      )
  )



