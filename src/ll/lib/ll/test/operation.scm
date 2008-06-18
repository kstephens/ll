(define <my-cons> (make <type> (list <object>) (list 'car 'cdr)))

(type-op-meth-alist <my-cons>)
; ()

(define my-car (make <operation>))
my-car
; #<<operation> my-car>

;; Single method on operation my-car will not
;; add method to <my-cons> type-op-meth-alist
;;

(add-method (my-car (<my-cons> car) self) car)
; #<<operation> my-car>

(type-op-meth-alist <my-cons>)
; ()

(define x (make <my-cons>))

(my-car x)
; #u


