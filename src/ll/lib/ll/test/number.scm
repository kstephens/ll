(load "ll_test.scm")

(let ((x #f))

; (%bc:debug)

'(
t "(get-type *bignum:min*)"
t "(number->string *bignum:min*)"
t "(negative? *bignum:min*)"
t "(zero?     *bignum:min*)"
t "(positive? *bignum:min*)"
t "(exact->inexact *bignum:min*)"
t "(number->string *bignum:max*)"
t "(exact->inexact *bignum:max*)"
t "(number->string (+ *bignum:max* 1))"
t "(gcd (+ *fixnum:max* 1) 2)"
t "(gcd 2 (+ *fixnum:max* 1))'
)

(newline)(write 'read)(newline)
(ll:test:assert <fixnum> (get-type 0))
(ll:test:assert <fixnum> (get-type 1))
(ll:test:assert <fixnum> (get-type 10))
(ll:test:assert <fixnum> (get-type -10))
(ll:test:assert <bignum> (get-type 123456789012345678901234567890))
(ll:test:assert <flonum> (get-type 123456789012345678901234567890.0))
(ll:test:assert <ratnum> (get-type 1/2))
(ll:test:assert <ratnum> (get-type 1/12345678910))
(ll:test:assert <ratnum> (get-type -1/12345678910))
(ll:test:assert <ratnum> (get-type 1/-12345678910))
(ll:test:assert <ratnum> (get-type -1/-12345678910))

(newline)(write 'numerical-tower)(newline)
(set! x -10)
(ll:test:assert #t (number?   x))
(ll:test:assert #t (real?     x))
(ll:test:assert #t (rational? x))
(ll:test:assert #t (integer?  x))
(set! x 123456789012345678901234567890)
(ll:test:assert #t (number?   x))
(ll:test:assert #t (real?     x))
(ll:test:assert #t (rational? x))
(ll:test:assert #t (integer?  x))
(set! x 1/2)
(ll:test:assert #t (number?   x))
(ll:test:assert #t (real?     x))
(ll:test:assert #t (rational? x))
(ll:test:assert #f (integer?  x))

(newline)(write 'number->string)(newline)
(ll:test:assert "0"
		(number->string 0))
(ll:test:assert "FF"      
		(number->string 255 16))
(ll:test:assert "11111111"      
		(number->string 255 2))
(ll:test:assert "-10"
		(number->string -10))
(ll:test:assert "-A"      
		(number->string -10 16))
(ll:test:assert "123456789012345678901234567890" 
		(number->string 123456789012345678901234567890))
(ll:test:assert "-123456789012345678901234567890" 
		(number->string -123456789012345678901234567890))

(newline)(write '*?)(newline)
(ll:test:assert #f       (negative? 0))
(ll:test:assert #t       (zero? 0))
(ll:test:assert #f       (positive? 0))
(ll:test:assert #t       (even? 0))
(ll:test:assert #f       (odd? 0))

(ll:test:assert #f       (negative?  10))
(ll:test:assert #f       (zero?      10))
(ll:test:assert #t       (positive?  10))
(ll:test:assert #t       (even?      10))
(ll:test:assert #f       (odd?       10))

(ll:test:assert #t       (negative? -11))
(ll:test:assert #f       (zero?     -11))
(ll:test:assert #f       (positive? -11))
(ll:test:assert #f       (even?     -11))
(ll:test:assert #t       (odd?      -11))

(ll:test:assert #f       (negative?  123456789012345678901234567890))
(ll:test:assert #f       (zero?      123456789012345678901234567890))
(ll:test:assert #t       (positive?  123456789012345678901234567890))
(ll:test:assert #t       (even?      123456789012345678901234567890))
(ll:test:assert #f       (odd?       123456789012345678901234567890))

(ll:test:assert #t       (negative? -123456789012345678901234567891))
(ll:test:assert #f       (zero?     -123456789012345678901234567891))
(ll:test:assert #f       (positive? -123456789012345678901234567891))
(ll:test:assert #f       (even?     -123456789012345678901234567891))
(ll:test:assert #t       (odd?      -123456789012345678901234567891))

(ll:test:assert #f       (negative?  123456789012345678901234567890.0))
(ll:test:assert #f       (zero?      123456789012345678901234567890.0))
(ll:test:assert #t       (positive?  123456789012345678901234567890.0))

(ll:test:assert #t       (negative? -123456789012345678901234567890.0))
(ll:test:assert #f       (zero?     -123456789012345678901234567890.0))
(ll:test:assert #f       (positive? -123456789012345678901234567890.0))

(ll:test:assert #f       (negative? 1/12345678910))
(ll:test:assert #f       (zero?     1/12345678910))
(ll:test:assert #t       (positive? 1/12345678910))

(ll:test:assert #t       (negative? -1/12345678910))
(ll:test:assert #f       (zero?     -1/12345678910))
(ll:test:assert #f       (positive? -1/12345678910))

(ll:test:assert #t       (negative? 1/-12345678910))
(ll:test:assert #f       (zero?     1/-12345678910))
(ll:test:assert #f       (positive? 1/-12345678910))

(ll:test:assert #t       (positive? -1/-12345678910))


(newline)(write '+)(newline)
(ll:test:assert 0 (+))
(ll:test:assert 1 (+ 1))
(ll:test:assert <fixnum> (get-type (+ *fixnum:max*)))
(ll:test:assert <bignum> (get-type (+ *fixnum:max* 1)))
(ll:test:assert <bignum> (get-type (+ *fixnum:max* 2)))
(ll:test:assert <bignum> (get-type (+ 1 *fixnum:max*)))
(ll:test:assert <bignum> (get-type (+ 2 *fixnum:max*)))
(ll:test:assert <bignum> (get-type (+ *fixnum:min* -1)))
(ll:test:assert <bignum> (get-type (+ *fixnum:min* -2)))
(ll:test:assert <bignum> (get-type (+ -1 *fixnum:min*)))
(ll:test:assert <bignum> (get-type (+ -2 *fixnum:min*)))
(ll:test:assert 246913578024691357802469135780 (+ 123456789012345678901234567890 123456789012345678901234567890))

(ll:test:assert #t (fixnum? (- *bignum:max* *bignum:max*)))

(newline)(write '-)(newline)
(ll:test:assert 0 (- 0))
(ll:test:assert -1 (- 1))
(ll:test:assert 1 (- -1))
(ll:test:assert <fixnum> (get-type (- *fixnum:max*)))
(ll:test:assert <fixnum> (get-type (- *fixnum:min*)))
(ll:test:assert -123456789012345678901234567890 (- 123456789012345678901234567890))
(ll:test:assert 0 (- 123456789012345678901234567890 123456789012345678901234567890))
(ll:test:assert 123456789012345678901234567890 (- 123456789012345678901234567890 0))
(ll:test:assert -123456789012345678901234567890 (- 0 123456789012345678901234567890 0))

(newline)(write '*)(newline)
(ll:test:assert 1 (*))
(ll:test:assert 2 (* 2))
(ll:test:assert 6 (* 2 3))
(ll:test:assert -6 (* 2 -3))
(ll:test:assert 246913578024691357802469135780 (* 2 123456789012345678901234567890))
(ll:test:assert 246913578024691357802469135780 (* 123456789012345678901234567890 2))

(newline)(write '/)(newline)
(ll:test:assert 1 (/ 1))
(ll:test:assert 0.5 (/ 2.0))
(ll:test:assert 123456789012345678901234567890 (/ 246913578024691357802469135780 2))

(newline)(write '<ratnum>)(newline)
(ll:test:assert 1/2 (/ 2))
(ll:test:assert 2 (/ 1/2))
(ll:test:assert 2 (/ 4 2))
(ll:test:assert 1/2 (/ 4 8))
(ll:test:assert 4/3 (/ 4 3))
(ll:test:assert 246913578024691357802469135780 (/ 123456789012345678901234567890 1/2))


)

(newline)'ok
