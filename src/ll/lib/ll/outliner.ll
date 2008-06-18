

(define-macro (ll:type:make typename supers slots)
  `(begin 
  	(define-constant ,typename (make ,supers ,slots))
	(add-method (initialize (,typename . ,slots) self . args)
	   (while (not (null? args))
	     (case (car args) 
	       ,(map (lambda (slot) 
	       	       `((,slot) 
		          (begin 
			    (set! args (cdr args))
			    (set! ,slot (car args))))) ,slots)))
	   (for ((s ,typename))
	     (super s initialize args)))))
	     
  
(define <ll:outliner:item>
  (make <type> 
    (list <object>)
    '(
      key 
      getter
      setter
      deleter
      )))
  
(define <ll:outliner:section
  (make <type>
    (list <ll:outliner:item>)
    '(collapsed items)))

(define <ll:outliner:item
  (make <type>
    (list <ll:outliner:section>)
    '(root window key_item_map)))


(define-constant ll:outliner (make <operation>))

(add-method (ll:outliner (<object>) (self))
  (let ((ol (get ll:outliner:object-outliner-map self)))
    (if ol
      (ol ll:outliner:refresh)
      (begin
        (set! ol (make <ll:outliner> 'root self))
        (ll:outliner:fill self ol)))
    ol))

(define-constant ll:outliner:fill (make <operation>))
(add-method (ll:outliner:fill (<vector>) (self outliner)
  (super ll:outliner:fill outliner)
  (let* ((sec (make <ll:outliner:section> 'vector-contents))
        ((i 0)))
	(for ((i 0) (set! i (+ 1 i)))
	   (ll:outliner:add-item (make <ll:outliner:item> 'key (make-locative-cell i) 'value (locative-vector-ref self i) 'setter))
	))
)

