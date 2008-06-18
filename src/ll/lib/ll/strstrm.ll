;$Id: strstrm.ll,v 1.1 1999/03/02 12:21:33 stephensk Exp $

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; <string-input-port>
;;

(define <string-input-port>
  (make <type> (list <input-port>) (list 'str 'pos)))

(add-method (initialize (<string-input-port> str pos) self s)
  (set! str s)
  (set! pos 0)
)

(add-method (port-impl (<string-output-port> str) self)
  str
)

(add-method (peek-char (<string-input-port> str pos) self)
  (if (< pos (string-length str))
    (string-ref str pos)
    port:$eos
  )
)

(add-method (read-char (<string-input-port> str pos) self)
  (if (< pos (string-length str))
    (begin 
      (string-ref str pos)
      (set! pos (+ pos 1))
    )
    port:$eos
  )
)


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; <string-output-port>
;;

(define <string-output-port>
  (make <type> (list <output-port>) (list 'str)))

(add-method (port-impl (<string-output-port> str) self)
  str
)

(add-method (initialize (<string-output-port> str) self . s)
  (set! str (if (null? s) 
                (make <mutable-string>) 
                (car s)))
)

(add-method (%output-char (<string-output-port> str) self c)
  (append-one! str c)
)

(add-method (%output-string (<string-output-port> str) self s)
  (append! str s)
)

