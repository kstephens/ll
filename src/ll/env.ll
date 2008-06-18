(define-constant <environment>
  (make <type>
     (list <object>)
     (list 'parent
           'values
	   'macros)))
	 
	 
(define %ll:value-binding (make <locatable-operation>))
(define %ll:macro-binding (make <locatable-operation>))

(add-method (%ll:value-binding (<environment> parent values) (env sym))
   (let* ((b (assq (values))))
      (if b
        b
	(error <unbound-variable-error> env sym))))
	
