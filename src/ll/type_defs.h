/******************************************************************/

     /* <properties-mixin> objects can perform property methods. */
ll_type_def(properties_mixin, type)
ll_type_def_supers(properties_mixin)
ll_type_def_supers_end(properties_mixin)
ll_type_def_slots(properties_mixin)
ll_type_def_slots_end(properties_mixin)
ll_type_def_end(properties_mixin)


/*
 * <not-equal-mixin> objects are never equal?.
 * See eq.c.
 */
ll_type_def(not_equal_mixin, type)
  ll_type_def_supers(not_equal_mixin)
  ll_type_def_supers_end(not_equal_mixin)
  ll_type_def_slots(not_equal_mixin)
  ll_type_def_slots_end(not_equal_mixin)
ll_type_def_end(not_equal_mixin)


/*
 * <not-equal-mixin> objects are never equal?.
 * See eq.c.
 */
ll_type_def(equal_mixin, type)
  ll_type_def_supers(equal_mixin)
  ll_type_def_supers_end(equal_mixin)
  ll_type_def_slots(equal_mixin)
  ll_type_def_slots_end(equal_mixin)
ll_type_def_end(equal_mixin)


/*
 * <binary-mixin> objects can have binary fields.
 * See eq.c.
 */
ll_type_def(binary_mixin, type)
  ll_type_def_supers(binary_mixin)
    ll_type_def_super(binary_mixin, not_equal_mixin)
  ll_type_def_supers_end(binary_mixin)
  ll_type_def_slots(binary_mixin)
  ll_type_def_slots_end(binary_mixin)
ll_type_def_end(binary_mixin)


/******************************************************************/

     /* <object> is the root of all instantiable objects */
ll_type_def(object, type)
ll_type_def_supers(object)
ll_type_def_supers_end(object)
ll_type_def_slots(object)
  ll_type_def_slot(object, ll_v, isa)
ll_type_def_slots_end(object)
  ll_type_def_option(object, top_wiredQ, "#t")
ll_type_def_end(object)

     /* <type> objects are the type of all objects */
ll_type_def(type, type)
ll_type_def_supers(type)
  ll_type_def_super(type, object)
  ll_type_def_super(type, properties_mixin)
ll_type_def_supers_end(type)
ll_type_def_slots(type)
  ll_type_def_slot(type, ll_v, supers)
     /* A list of super types. */
  ll_type_def_slot(type, ll_v, slots)
     /* A list of (<slot_name> <slot_offset> . <slot_type>) */

  ll_type_def_slot(type, ll_v, size)
     /* The size of this object. */

  ll_type_def_slot(type, ll_v, slots_size)
     /* The size of this type's slot block. */

#if ll_USE_OP_METH_ALIST
  ll_type_def_slot(type, ll_v, op_meth_alist)
     /* A map of (<op> . <meth>) */
#endif

  ll_type_def_slot(type, ll_v, type_offset_alist)
     /* A map of (<type> . <slot_block_offset>) */

  ll_type_def_slot(type, ll_v, top_wiredQ)
     /* The type is top-wired by the internals. */

  ll_type_def_slot(type, ll_v, immutableQ)
     /* The type is immutable.  No methods can be added, removed or changed. */

  ll_type_def_slot(type, ll_v, tester)
     /* <type>? */

  ll_type_def_slot(type, ll_v, coercer)
     /* (coercer <type>) */

  ll_type_def_slot(type, ll_v, typechecker)
     /* (typechecker <type>) */

  ll_type_def_slot(type, ll_v, properties)
     /* Additional properties. */
ll_type_def_slots_end(type)
  ll_type_def_option(type, top_wiredQ, "#t")
ll_type_def_end(type)


/******************************************************************/

ll_type_def(singleton_type, type)
ll_type_def_supers(singleton_type)
  ll_type_def_super(singleton_type, type)
ll_type_def_supers_end(singleton_type)
ll_type_def_slots(singleton_type)
  ll_type_def_slot(singleton_type, ll_v, instance)
ll_type_def_slots_end(singleton_type)
ll_type_def_end(singleton_type)


/******************************************************************/

     /* an object of the <immutable> type returns #t when sent immutable? */
ll_type_def(immutable, type)
ll_type_def_supers(immutable)
ll_type_def_supers_end(immutable)
ll_type_def_slots(immutable)
ll_type_def_slots_end(immutable)
ll_type_def_end(immutable)

     /* an object of the <mutable> type returns #f when send immutable? */
ll_type_def(mutable, type)
ll_type_def_supers(mutable)
ll_type_def_supers_end(mutable)
ll_type_def_slots(mutable)
ll_type_def_slots_end(mutable)
ll_type_def_end(mutable)

/******************************************************************/

     /* an object of the <constant> type is compiled as a literal value */
ll_type_def(constant, type)
ll_type_def_supers(constant)
  ll_type_def_super(constant, immutable)
ll_type_def_supers_end(constant)
ll_type_def_slots(constant)
ll_type_def_slots_end(constant)
ll_type_def_end(constant)

     /* an object of the <immediate> type does not require storage in the heap */
ll_type_def(immediate, type)
ll_type_def_supers(immediate)
  ll_type_def_super(immediate, constant)
ll_type_def_supers_end(immediate)
ll_type_def_slots(immediate)
ll_type_def_slots_end(immediate)
ll_type_def_end(immediate)

     /* Only one object of the <singleton> type exists. */
ll_type_def(singleton, type)
ll_type_def_supers(singleton)
ll_type_def_supers_end(singleton)
ll_type_def_slots(singleton)
ll_type_def_slots_end(singleton)
ll_type_def_end(singleton)

/******************************************************************/

ll_type_def(pointer,type)
ll_type_def_supers(pointer)
  ll_type_def_super(pointer, immediate)
  ll_type_def_super(pointer, object)
ll_type_def_supers_end(pointer)
ll_type_def_slots(pointer)
ll_type_def_slots_end(pointer)
ll_type_def_end(pointer)

ll_type_def(locative,type)
ll_type_def_supers(locative)
  ll_type_def_super(locative, immediate)
  ll_type_def_super(locative, object)
ll_type_def_supers_end(locative)
ll_type_def_slots(locative)
ll_type_def_slots_end(locative)
ll_type_def_end(locative)

/******************************************************************/

     /* <procedure> is a mixin of all objects that are applicable.  Currently the only instantiable <procedure> type is <operation> */

ll_type_def(procedure, type)
ll_type_def_supers(procedure)
ll_type_def_supers_end(procedure)
ll_type_def_slots(procedure)
ll_type_def_slots_end(procedure)
ll_type_def_end(procedure)

     /* objects of the <operation> type are the only objects that can be in the car-position of a function call. i.e. (car '(foo bar)). the value of "car" is a <operation> object. */

ll_type_def(operation, type)
ll_type_def_supers(operation)
  ll_type_def_super(operation, procedure)
  ll_type_def_super(operation, object)
  ll_type_def_super(operation, properties_mixin)
ll_type_def_supers_end(operation)
ll_type_def_slots(operation)
  ll_type_def_slot(operation, ll_v, lambdaQ)
     /* A method if a method has been added to only one type. */
  ll_type_def_slot(operation, ll_v, lambda_type)
     /* The <type> for the lambda? */
  ll_type_def_slot(operation, ll_v, cache_type)
     /* The <type> that the last full search was tried against. */
  ll_type_def_slot(operation, ll_v, cache_impl)
     /* The <type> that implemented the <method> last found. */
  ll_type_def_slot(operation, ll_v, cache_offset)
     /* The offset of the cache-impl in cache-type. */
  ll_type_def_slot(operation, ll_v, cache_method)
     /* The <method> of the last cached lookup. */
#if ll_USE_TYPE_METH_ALIST
  ll_type_def_slot(operation, ll_v, type_meth_alist)
     /* Mapping of types to methods. */
#endif
  ll_type_def_slot(operation, ll_v, immutableQ)
     /* If true, no methods can be added or removed. */
  ll_type_def_slot(operation, ll_v, properties)
     /* Additional properties. */
#if ll_USE_OPERATION_VERSION
  ll_type_def_slot(operation, ll_v, version)
     /* Incremented if method is added or removed. */
#endif
ll_type_def_slots_end(operation)
  ll_type_def_option(operation, top_wiredQ, "#t")
ll_type_def_end(operation)

     /* <settable-operations> are <operation> objects that respond to the "setter" operation.  For example "car" is a <settable-operation>; (setter car) returns "set-car!".  "(set! (car foo) bar)" macro expands to "((setter car) foo bar)" */

ll_type_def(settable_operation,type)
ll_type_def_supers(settable_operation)
  ll_type_def_super(settable_operation, operation)
ll_type_def_supers_end(settable_operation)
ll_type_def_slots(settable_operation)
  ll_type_def_slot(settable_operation, ll_v, setter)
ll_type_def_slots_end(settable_operation)
ll_type_def_end(settable_operation)

     /* <locatable-operations> are <settable-operation> objects that respond to the "locater" operation.  For example "car" is a <locatable-operation>; (locator car) returns "locative-car".  "(make-locative (car foo))" macro expands to "((locater car) foo)" */
ll_type_def(locatable_operation,type)
ll_type_def_supers(locatable_operation)
  ll_type_def_super(locatable_operation, settable_operation)
ll_type_def_supers_end(locatable_operation)
ll_type_def_slots(locatable_operation)
  ll_type_def_slot(locatable_operation, ll_v, locater)
ll_type_def_slots_end(locatable_operation)
ll_type_def_end(locatable_operation)

/******************************************************************/

     /* The <undefined> object, #u, is used for all uninitalized values */
ll_type_def(undefined, type)
ll_type_def_supers(undefined)
  ll_type_def_super(undefined, constant)
  ll_type_def_super(undefined, object)
ll_type_def_supers_end(undefined)
ll_type_def_slots(undefined)
ll_type_def_slots_end(undefined)
ll_type_def_end(undefined)

     /* The <unspecified> object is used for all Scheme procedures and primitives that do not have a specified return value */
ll_type_def(unspecified, type)
ll_type_def_supers(unspecified)
  ll_type_def_super(unspecified, constant)
  ll_type_def_super(unspecified, object)
ll_type_def_supers_end(unspecified)
ll_type_def_slots(unspecified)
ll_type_def_slots_end(unspecified)
ll_type_def_end(unspecified)

/******************************************************************/

/* <message> objects are allocated on a stack during each <operation> object application.  */
ll_type_def(message, type)
ll_type_def_supers(message)
  ll_type_def_super(message, binary_mixin)
  ll_type_def_super(message, object)
ll_type_def_supers_end(message)
ll_type_def_slots(message)
  ll_type_def_slot(message, ll_v, op)
     /* The <operation> searched for. */
  ll_type_def_slot(message, int, argc)
     /* Argument list length; if argc == 0, rcvr's is assumed to be <object>. */
  ll_type_def_slot(message, ll_v*, argv)
     /* Argument list; argv[0] is the rcvr. */

  ll_type_def_slot(message, ll_tsa_stack_buffer*, argv_sb)
     /* The stack buffer that contains argv. */

  ll_type_def_slot(message, ll_v, type)
     /* The type the method was found in. */
  ll_type_def_slot(message, ll_v, type_offset)
     /* The offset of type's ivar block in the rcvr. */
  ll_type_def_slot(message, ll_v, meth)
     /* The method that was found by the lookup. */

  ll_type_def_slot(message, ll_v, db_at_rtn)
     /* Envoke the debugger when this frame is returned from. */

#if ll_USE_FILE_LINE
  ll_type_def_slot(message, char*, file)
     /* The file of the caller. */ 
  ll_type_def_slot(message, int, line)
     /* The line of the caller. */
#endif
ll_type_def_slots_end(message)
  ll_type_def_option(message, top_wiredQ, "#t")
ll_type_def_end(message)


/* <stack-buffer> objects are used to manage the value and message stacks.  */
ll_type_def(stack_buffer, type)
ll_type_def_supers(stack_buffer)
  ll_type_def_super(stack_buffer, binary_mixin)
  ll_type_def_super(stack_buffer, object)
ll_type_def_supers_end(stack_buffer)
ll_type_def_slots(stack_buffer)

  ll_type_def_slot(stack_buffer, ll_v*, sp)
     /* The current index into the buffer.
	  peek()  => *sp
          push(v) => *(-- sp)
      v = pop()   => *(sp ++)
     */

  ll_type_def_slot(stack_buffer, size_t, size)
     /* The size of this buffer in element_size units. */

  ll_type_def_slot(stack_buffer, size_t, element_size)
     /* The size of each element in ll_v units. */

  ll_type_def_slot(stack_buffer, ll_v*, beg)
     /* The <vector> holding the elements of the stack. */

  ll_type_def_slot(stack_buffer, ll_v*, end)
     /* The location after the end of the stack buffer,
      * points at the "bottom" of the stack,
      * which is the high address.
      */

  ll_type_def_slot(stack_buffer, ll_tsa_stack_buffer*, prev)
     /* The previous stack buffer. */

ll_type_def_slots_end(stack_buffer)
  ll_type_def_option(stack_buffer, top_wiredQ, "#t")
ll_type_def_end(stack_buffer)


/******************************************************************/

     /* <method> objects are the code that is assocated for a <type> and <operation> pair. */
ll_type_def(method, type)
ll_type_def_supers(method)
  ll_type_def_super(method, binary_mixin)
  ll_type_def_super(method, object)
  ll_type_def_super(method, properties_mixin)
ll_type_def_supers_end(method)
ll_type_def_slots(method)
  ll_type_def_slot(method, ll_func, func)
     /* The internal function that will handle this method. */
  ll_type_def_slot(method, ll_v, formals)
     /* The formal argument list. */
  ll_type_def_slot(method, ll_v, code) 
     /* The code vector: for byte-code methods is a string, for primitives this is a symbol. */ 
  ll_type_def_slot(method, ll_v, properties)
     /* Various options */
  ll_type_def_slot(method, ll_v, consts)
    /* A vector containing constant values */
  ll_type_def_slot(method, ll_v, env)
    /* The environment vector */
ll_type_def_slots_end(method)
  ll_type_def_option(method, top_wiredQ, "#t")
ll_type_def_end(method)


/* <primitive> objects are <method> objects that are implemented in C */
ll_type_def(primitive, type)
ll_type_def_supers(primitive)
  ll_type_def_super(primitive, method)
ll_type_def_supers_end(primitive)
ll_type_def_slots(primitive)
ll_type_def_slots_end(primitive)
ll_type_def_end(primitive)

		
/* <byte-code-method> objects are <method> objects that are implemented as byte code instructions */
ll_type_def(byte_code_method,type)
ll_type_def_supers(byte_code_method)
#if ll_USE_LCACHE
  ll_type_def_super(method, binary_mixin)
#endif
  ll_type_def_super(byte_code_method, method)
ll_type_def_supers_end(byte_code_method)
ll_type_def_slots(byte_code_method)
#if ll_USE_LCACHE
  ll_type_def_slot(byte_code_method, ll_lcache_vector*, lcache_vector)
     /* A vector of call site lookup caches in method. */
#endif
ll_type_def_slots_end(byte_code_method)
#if ll_USE_LCACHE
  ll_type_def_option(byte_code_method, top_wiredQ, "#t")
#endif
ll_type_def_end(byte_code_method)



/* <stack-state> objects are use to save stack state 
 * for catch and continuation.
 * See stack.c 
 */
ll_type_def(stack_state, type)
  ll_type_def_supers(stack_state)
  ll_type_def_supers_end(stack_state)
ll_type_def_slots(stack_state)
  ll_type_def_slot(stack_state, ll_v, val_sp)
  ll_type_def_slot(stack_state, ll_v, val_stack_buffer)
  ll_type_def_slot(stack_state, ll_v, ar_sp)
  ll_type_def_slot(stack_state, ll_v, ar_sp_base)
  ll_type_def_slot(stack_state, ll_v, ar_sp_bottom)
  ll_type_def_slot(stack_state, ll_v, fluid_bindings)
  ll_type_def_slot(stack_state, ll_v, current_catch)
ll_type_def_slots_end(stack_state)
ll_type_def_end(stack_state)


/* <catch> objects are methods that do one-way escapes to (catch ...) handlers */
ll_type_def(catch, type)
ll_type_def_supers(catch)
  ll_type_def_super(catch, binary_mixin)
  ll_type_def_super(catch, method)
  ll_type_def_super(catch, stack_state)
ll_type_def_supers_end(catch)
ll_type_def_slots(catch)
  ll_type_def_slot(catch, ll_v, type)
  ll_type_def_slot(catch, ll_v, op)
  ll_type_def_slot(catch, ll_v, previous_catch)
  ll_type_def_slot(catch, ll_v, validQ)
  ll_type_def_slot(catch, ll_v, value)
  ll_type_def_slot(catch, jmp_buf, _jb)
ll_type_def_slots_end(catch)
ll_type_def_end(catch)


ll_type_def(continuation_method,type)
ll_type_def_supers(continuation_method)
  ll_type_def_super(continuation_method, binary_mixin)
  ll_type_def_super(continuation_method, method)
  ll_type_def_super(continuation_method, stack_state)
ll_type_def_supers_end(continuation_method)
ll_type_def_slots(continuation_method)
  ll_type_def_slot(continuation_method, CCont*, cc)
ll_type_def_slots_end(continuation_method)
ll_type_def_end(continuation_method)


/* A <trace-method> prints a synopsis of the <operation> application when they are invoked. */
ll_type_def(trace_method, type)
ll_type_def_supers(trace_method)
  ll_type_def_super(trace_method, method)
ll_type_def_supers_end(trace_method)
ll_type_def_slots(trace_method)
  ll_type_def_slot(trace_method, ll_v, type)
  ll_type_def_slot(trace_method, ll_v, op)
  ll_type_def_slot(trace_method, ll_v, meth)
ll_type_def_slots_end(trace_method)
ll_type_def_end(trace_method)


/******************************************************************/


/* There are two <boolean> objects: #t, the true value, #f the false value. */
ll_type_def(boolean, type)
ll_type_def_supers(boolean)
  ll_type_def_super(boolean, constant)
  ll_type_def_super(boolean, object)
ll_type_def_supers_end(boolean)
ll_type_def_slots(boolean)
ll_type_def_slots_end(boolean)
ll_type_def_end(boolean)


/******************************************************************/


/* There is only one <eof-object> object (eof-object) */
ll_type_def(eof_object, type)
ll_type_def_supers(eof_object)
  ll_type_def_super(eof_object, constant)
  ll_type_def_super(eof_object, object)
  ll_type_def_super(eof_object, singleton)
ll_type_def_supers_end(eof_object)
ll_type_def_slots(eof_object)
ll_type_def_slots_end(eof_object)
ll_type_def_end(eof_object)


/******************************************************************/


/* <number> objects are all <constants> objects. */
ll_type_def(number, type)
ll_type_def_supers(number)
  ll_type_def_super(number, constant)
  ll_type_def_super(number, object)
ll_type_def_supers_end(number)
ll_type_def_slots(number)
ll_type_def_slots_end(number)
ll_type_def_end(number)


/* exact and inexact mixins */
ll_type_def(exact, type)
ll_type_def_supers(exact)
ll_type_def_supers_end(exact)
ll_type_def_slots(exact)
ll_type_def_slots_end(exact)
ll_type_def_end(exact)


ll_type_def(inexact, type)
ll_type_def_supers(inexact)
ll_type_def_supers_end(inexact)
ll_type_def_slots(inexact)
ll_type_def_slots_end(inexact)
ll_type_def_end(inexact)


ll_type_def(complex, type)
ll_type_def_supers(complex)
  ll_type_def_super(complex, number)
ll_type_def_supers_end(complex)
ll_type_def_slots(complex)
ll_type_def_slots_end(complex)
ll_type_def_end(complex)


ll_type_def(real, type)
ll_type_def_supers(real)
  ll_type_def_super(real, complex)
ll_type_def_supers_end(real)
ll_type_def_slots(real)
ll_type_def_slots_end(real)
ll_type_def_end(real)


ll_type_def(rational, type)
ll_type_def_supers(rational)
  ll_type_def_super(rational, real)
ll_type_def_supers_end(rational)
ll_type_def_slots(rational)
ll_type_def_slots_end(rational)
ll_type_def_end(rational)


ll_type_def(integer, type)
ll_type_def_supers(integer)
  ll_type_def_super(integer, rational)
ll_type_def_supers_end(integer)
ll_type_def_slots(integer)
ll_type_def_slots_end(integer)
ll_type_def_end(integer)


/* Actual number implementation */

/* Fixnum are immediate values with a #b00 low tag */
ll_type_def(fixnum, type)
ll_type_def_supers(fixnum)
  ll_type_def_super(fixnum, immediate)
  ll_type_def_super(fixnum, exact)
  ll_type_def_super(fixnum, integer)
ll_type_def_supers_end(fixnum)
ll_type_def_slots(fixnum)
ll_type_def_slots_end(fixnum)
ll_type_def_end(fixnum)

/* <flonums> are immediate values with a #b10 low tag */
ll_type_def(flonum, type)
ll_type_def_supers(flonum)
  ll_type_def_super(flonum, immediate)
  ll_type_def_super(flonum, inexact)
  ll_type_def_super(flonum, real)
ll_type_def_supers_end(flonum)
ll_type_def_slots(flonum)
ll_type_def_slots_end(flonum)
ll_type_def_end(flonum)


/* <ratnum> are rationals. */
ll_type_def(ratnum, type)
ll_type_def_supers(ratnum)
  ll_type_def_super(ratnum, exact)
  ll_type_def_super(ratnum, rational)
ll_type_def_supers_end(ratnum)
ll_type_def_slots(ratnum)
  ll_type_def_slot(ratnum, ll_v, numerator)
  ll_type_def_slot(ratnum, ll_v, denominator)
ll_type_def_slots_end(ratnum)
ll_type_def_end(ratnum)


/* <bignum> are integers that cannot fit in <fixnum>. */
ll_type_def(bignum, type)
ll_type_def_supers(bignum)
  ll_type_def_super(bignum, exact)
  ll_type_def_super(bignum, integer)
ll_type_def_supers_end(bignum)
ll_type_def_slots(bignum)
  ll_type_def_slot(bignum, mpz_t, n)
ll_type_def_slots_end(bignum)
ll_type_def_end(bignum)


/******************************************************************/


/* <sequence> objects respond to (map <procedure> <sequence> ... ) and the like. */
ll_type_def(sequence, type)
ll_type_def_supers(sequence)
  ll_type_def_super(sequence, immutable)
ll_type_def_supers_end(sequence)
ll_type_def_slots(sequence)
ll_type_def_slots_end(sequence)
ll_type_def_end(sequence)


ll_type_def(mutable_sequence, type)
ll_type_def_supers(mutable_sequence)
  ll_type_def_super(mutable_sequence, mutable)
  ll_type_def_super(mutable_sequence, sequence)
ll_type_def_supers_end(mutable_sequence)
ll_type_def_slots(mutable_sequence)
ll_type_def_slots_end(mutable_sequence)
ll_type_def_end(mutable_sequence)


/******************************************************************/


     /* <list> objects are the empty list (the <null> object) and any <cons> object */
ll_type_def(list, type)
ll_type_def_supers(list)
  ll_type_def_super(list, equal_mixin)
  ll_type_def_super(list, sequence)
ll_type_def_supers_end(list)
ll_type_def_slots(list)
ll_type_def_slots_end(list)
ll_type_def_end(list)


ll_type_def(mutable_list, type)
ll_type_def_supers(mutable_list)
  ll_type_def_super(mutable_list, mutable_sequence)
  ll_type_def_super(mutable_list, list)
ll_type_def_supers_end(mutable_list)
ll_type_def_slots(mutable_list)
ll_type_def_slots_end(mutable_list)
ll_type_def_end(mutable_list)


/******************************************************************/


ll_type_def(null, type)
ll_type_def_supers(null)
  ll_type_def_super(null, list)
  ll_type_def_super(null, singleton)
  ll_type_def_super(null, object)
ll_type_def_supers_end(null)
ll_type_def_slots(null)
ll_type_def_slots_end(null)
ll_type_def_end(null)


ll_type_def(pair, type)
ll_type_def_supers(pair)
  ll_type_def_super(pair, list)
  ll_type_def_super(pair, object)
ll_type_def_supers_end(pair)
ll_type_def_slots(pair)
  ll_type_def_slot(pair, ll_v, car)
  ll_type_def_slot(pair, ll_v, cdr)
ll_type_def_slots_end(pair)
ll_type_def_end(pair)


ll_type_def(mutable_pair, type)
ll_type_def_supers(mutable_pair)
  ll_type_def_super(mutable_pair, mutable_list)
  ll_type_def_super(mutable_pair, pair)
ll_type_def_supers_end(mutable_pair)
ll_type_def_slots(mutable_pair)
ll_type_def_slots_end(mutable_pair)
ll_type_def_end(mutable_pair)


/******************************************************************/


     /* <char> objects are the elements of <string> objects. */
ll_type_def(char, type)
ll_type_def_supers(char)
  ll_type_def_super(char, constant)
  ll_type_def_super(char, object)
ll_type_def_supers_end(char)
ll_type_def_slots(char)
  ll_type_def_slot(char, ll_v, value)
ll_type_def_slots_end(char)
ll_type_def_end(char)


/******************************************************************/


     /* <string> objects are vectors of <char> objects */
ll_type_def(string, type)
ll_type_def_supers(string)
  ll_type_def_super(string, binary_mixin)
  ll_type_def_super(string, constant)
  ll_type_def_super(string, sequence)
  ll_type_def_super(string, object)
ll_type_def_supers_end(string)
ll_type_def_slots(string)
  ll_type_def_slot(string, char*, array)
  ll_type_def_slot(string, size_t, length)
ll_type_def_slots_end(string)
ll_type_def_end(string)


ll_type_def(mutable_string, type)
ll_type_def_supers(mutable_string)
  ll_type_def_super(mutable_string, mutable_sequence)
  ll_type_def_super(mutable_string, string)
ll_type_def_supers_end(mutable_string)
ll_type_def_slots(mutable_string)
ll_type_def_slots_end(mutable_string)
ll_type_def_end(mutable_string)


/******************************************************************/


     /* <vector> objects are vectors of any <object> */
ll_type_def(vector, type)
ll_type_def_supers(vector)
  ll_type_def_super(vector, binary_mixin)
  ll_type_def_super(vector, sequence)
  ll_type_def_super(vector, object)
ll_type_def_supers_end(vector)
ll_type_def_slots(vector)
  ll_type_def_slot(vector, ll_v*, array)
  ll_type_def_slot(vector, size_t, length)
ll_type_def_slots_end(vector)
ll_type_def_end(vector)


ll_type_def(mutable_vector, type)
ll_type_def_supers(mutable_vector)
  ll_type_def_super(mutable_vector, mutable_sequence)
  ll_type_def_super(mutable_vector, vector)
ll_type_def_supers_end(mutable_vector)
ll_type_def_slots(mutable_vector)
ll_type_def_slots_end(mutable_vector)
ll_type_def_end(mutable_vector)


/******************************************************************/


     /* <symbols> are immutable names such that:
(define s1 "aname")
(define s2 (string-copy s1))
(eq? (string->symbol s1) (string->symbol s2)) is always #t
*/

ll_type_def(symbol, type)
ll_type_def_supers(symbol)
  ll_type_def_super(symbol, object)
  ll_type_def_super(symbol, properties_mixin)   
ll_type_def_supers_end(symbol)
ll_type_def_slots(symbol)
  ll_type_def_slot(symbol, ll_v, name)
  ll_type_def_slot(symbol, ll_v, properties)
ll_type_def_slots_end(symbol)
ll_type_def_end(symbol)


/******************************************************************/

     /* <port> objects are used for extern input and output. */
ll_type_def(port, type)
ll_type_def_supers(port)
  ll_type_def_super(port, sequence)
  ll_type_def_super(port, object)
ll_type_def_supers_end(port)
ll_type_def_slots(port)
  ll_type_def_slot(port, ll_v, impl)
  ll_type_def_slot(port, ll_v, close)
  ll_type_def_slot(port, ll_v, ident)
ll_type_def_slots_end(port)
ll_type_def_end(port)


ll_type_def(input_port, type)
ll_type_def_supers(input_port)
  ll_type_def_super(input_port, port)
ll_type_def_supers_end(input_port)
ll_type_def_slots(input_port)
ll_type_def_slots_end(input_port)
ll_type_def_end(input_port)


ll_type_def(output_port, type)
ll_type_def_supers(output_port)
  ll_type_def_super(output_port, mutable_sequence)
  ll_type_def_super(output_port, port)
ll_type_def_supers_end(output_port)
ll_type_def_slots(output_port)
ll_type_def_slots_end(output_port)
ll_type_def_end(output_port)


#if 0
ll_type_def(io_port, type)
ll_type_def_supers(io_port)
  ll_type_def_super(io_port, input_port)
  ll_type_def_super(io_port, output_port)
ll_type_def_supers_end(io_port)
ll_type_def_slots(io_port)
ll_type_def_slots_end(io_port)
ll_type_def_end(io_port)
#endif


/******************************************************************/

     /* <file-port> objects are used for input and output to files. */
ll_type_def(file_port, type)
ll_type_def_supers(file_port)
  ll_type_def_super(file_port, port)
ll_type_def_supers_end(file_port)
ll_type_def_slots(file_port)
ll_type_def_slots_end(file_port)
ll_type_def_end(file_port)


ll_type_def(input_file_port, type)
ll_type_def_supers(input_file_port)
  ll_type_def_super(input_file_port, input_port)
  ll_type_def_super(input_file_port, file_port)
ll_type_def_supers_end(input_file_port)
ll_type_def_slots(input_file_port)
ll_type_def_slots_end(input_file_port)
ll_type_def_end(input_file_port)


ll_type_def(output_file_port, type)
ll_type_def_supers(output_file_port)
  ll_type_def_super(output_file_port, output_port)
  ll_type_def_super(output_file_port, file_port)
ll_type_def_supers_end(output_file_port)
ll_type_def_slots(output_file_port)
ll_type_def_slots_end(output_file_port)
ll_type_def_end(output_file_port)


/******************************************************************/
/* Handle circular object writes. */


ll_type_def(write_port, type)
ll_type_def_supers(write_port)
  ll_type_def_super(write_port, output_port)
ll_type_def_supers_end(write_port)
ll_type_def_slots(write_port)
  ll_type_def_slot(write_port, ll_v, mode)
  ll_type_def_slot(write_port, ll_v, obj_ref_alist)
  ll_type_def_slot(write_port, ll_v, obj_id_next)
  ll_type_def_slot(write_port, ll_v, obj_id_alist)
ll_type_def_slots_end(write_port)
ll_type_def_end(write_port)


/******************************************************************/


ll_type_def(readline_port, type)
ll_type_def_supers(readline_port)
  ll_type_def_super(readline_port, input_port)
ll_type_def_supers_end(readline_port)
ll_type_def_slots(readline_port)
  ll_type_def_slot(readline_port, ll_v, buffer)
  ll_type_def_slot(readline_port, ll_v, prompt_buffer)
  ll_type_def_slot(readline_port, ll_v, prompt_port)
ll_type_def_slots_end(readline_port)
ll_type_def_end(readline_port)


/******************************************************************/


ll_type_def(null_input_port, type)
ll_type_def_supers(null_input_port)
  ll_type_def_super(null_input_port, input_port)
ll_type_def_supers_end(null_input_port)
ll_type_def_slots(null_input_port)
ll_type_def_slots_end(null_input_port)
ll_type_def_end(null_input_port)


ll_type_def(null_output_port, type)
ll_type_def_supers(null_output_port)
  ll_type_def_super(null_output_port, output_port)
ll_type_def_supers_end(null_output_port)
ll_type_def_slots(null_output_port)
ll_type_def_slots_end(null_output_port)
ll_type_def_end(null_output_port)


/******************************************************************/

     /* <environment> objects contain all global bindings and macro definitions. */
ll_type_def(environment, type)
ll_type_def_supers(environment)
  ll_type_def_super(environment, object)
ll_type_def_supers_end(environment)
ll_type_def_slots(environment)
  ll_type_def_slot(environment, ll_v, bindings)
     /* A vector of <binding> objects */
ll_type_def_slots_end(environment)
ll_type_def_end(environment)


     /* <binding> objects are stored in the <environment> object's binding slot.  The contain the variables global value, macro function, and property list. */
ll_type_def(binding, type)
ll_type_def_supers(binding)
  ll_type_def_super(binding, object)
  ll_type_def_super(binding, properties_mixin)
ll_type_def_supers_end(binding)
ll_type_def_slots(binding)
  ll_type_def_slot(binding, ll_v, symbol)
     /* The binding's symbol key */

  ll_type_def_slot(binding, ll_v, value)
     /* The binding's internal value */

  ll_type_def_slot(binding, ll_v, locative)
     /* The binding's value locative */

  ll_type_def_slot(binding, ll_v, readonlyQ)
     /* The binding's value is readonly? */

  ll_type_def_slot(binding, ll_v, macro)
     /* The binding's macro expander */

  ll_type_def_slot(binding, ll_v, properties)
     /* The binding's properties list */

ll_type_def_slots_end(binding)
  ll_type_def_option(binding, top_wiredQ, "#t")
ll_type_def_end(binding)


/******************************************************************/

     /* <error> objects are generated upon internal and user error conditions. */
ll_type_def(error, type)
ll_type_def_supers(error)
  ll_type_def_super(error, object)
  ll_type_def_super(error, properties_mixin)
ll_type_def_supers_end(error)
ll_type_def_slots(error)
  ll_type_def_slot(error, ll_v, ar)
     /* The message that created the error */
  ll_type_def_slot(error, ll_v, properties)
     /* The properties of the error. */
ll_type_def_slots_end(error)
ll_type_def_end(error)


     /* Fatal errors will always report and kill the current application. */
ll_type_def(fatal_error, type)
ll_type_def_supers(fatal_error)
  ll_type_def_super(fatal_error, error)
ll_type_def_supers_end(fatal_error)
ll_type_def_slots(fatal_error)
ll_type_def_slots_end(fatal_error)
ll_type_def_end(fatal_error)


     /* Recoverable errors will invoke the debugger and return back to the error creator. */
ll_type_def(recoverable_error, type)
ll_type_def_supers(recoverable_error)
  ll_type_def_super(recoverable_error, error)
ll_type_def_supers_end(recoverable_error)
ll_type_def_slots(recoverable_error)
ll_type_def_slots_end(recoverable_error)
ll_type_def_end(recoverable_error)


     /* ll_e() error types */
#ifndef ll_e_def

#ifndef ll_e_NAME
#define ll_e_NAME(NAME,SUPER) NAME##_##SUPER
#endif

#define _ll_type_def_simple(NAME, SUPER) \
ll_type_def(NAME, type) \
ll_type_def_supers(NAME) \
  ll_type_def_super(NAME, SUPER) \
ll_type_def_supers_end(NAME) \
ll_type_def_slots(NAME) \
ll_type_def_slots_end(NAME) \
ll_type_def_end(NAME)

#define _ll_e_def(NAME,SUPER) _ll_type_def_simple(NAME,SUPER)

#define ll_e_def(NAME,SUPER) _ll_e_def(ll_e_NAME(NAME,SUPER),SUPER)

#include "ll/errors.h"

#undef _ll_type_def_simple
#undef _ll_e_def
#endif


/******************************************************************/

     /* <%ir> objects are created by the byte-code compiler to represent lexical closures and binding contours. */
ll_type_def(_ir, type)
ll_type_def_supers(_ir)
  ll_type_def_super(_ir, object)
  ll_type_def_super(_ir, properties_mixin)
ll_type_def_supers_end(_ir)
ll_type_def_slots(_ir)
   /* Inputs */
  ll_type_def_slot(_ir, ll_v, formals)
     /* A formal parameter list (last cdr may be the rest-arg) */
  ll_type_def_slot(_ir, ll_v, slots)
     /* A list of slot names (instance variables) */
  ll_type_def_slot(_ir, ll_v, body)
     /* A list of sexprs */
  ll_type_def_slot(_ir, ll_v, parent)
     /* Our lexical parent %ir object */

  /* Computed slots */
  ll_type_def_slot(_ir, ll_v, rest_argQ)
     /* The name of our rest-arg */
  ll_type_def_slot(_ir, ll_v, env)
     /* The exported environment */
  ll_type_def_slot(_ir, ll_v, env_exportedQ)
     /* True if the environment has been exported. */
  ll_type_def_slot(_ir, ll_v, env_length)
     /* The length of our exported environment */
  ll_type_def_slot(_ir, ll_v, imports)
     /* The imported environment */
     
  ll_type_def_slot(_ir, ll_v, argc)
     /* The number of arguments, minus the rest-arg */
  ll_type_def_slot(_ir, ll_v, n_locals)
     /* The number of locals, we need */
  ll_type_def_slot(_ir, ll_v, max_n_locals)
     /* The maximum number of locals we need for us and our car-pos lambdas */
  ll_type_def_slot(_ir, ll_v, probe_pos)
     /* The position of the stack xsprobe instruction. */

  ll_type_def_slot(_ir, ll_v, scope)
     /* A list of variable scopes */
  ll_type_def_slot(_ir, ll_v, car_posQ)
     /* Are we a car-position lambda? */

  ll_type_def_slot(_ir, ll_v, environment)
     /* The global environment we are compiling in. */

  /* Internal state */
  ll_type_def_slot(_ir, ll_v, properties)
     /* Compilation options. */

  /* Outputs */
  ll_type_def_slot(_ir, ll_v, consts)
     /* A vector containing all constant values. */

  ll_type_def_slot(_ir, ll_v, code_ir)
    /* The IR that should recieve emits */

  ll_type_def_slot(_ir, ll_v, code)
     /* A string containing all our byte-code. */

  ll_type_def_slot(_ir, ll_v, code_gen)
     /* A vector containing high-level assembly (used for diagnostics). */

  ll_type_def_slot(_ir, ll_v, call_site_count)
     /* The number of call sites in the byte-code. */

  ll_type_def_slot(_ir, ll_v, props)
     /* An assoc for the methods properties. */
ll_type_def_slots_end(_ir)
ll_type_def_end(_ir)


/******************************************************************/


ll_type_def(debugger, type)
ll_type_def_supers(debugger)
  ll_type_def_super(debugger, object)
  ll_type_def_super(debugger, properties_mixin)
ll_type_def_supers_end(debugger)
ll_type_def_slots(debugger)
  ll_type_def_slot(debugger, ll_v, properties)
     /* Current debugger state. */
  ll_type_def_slot(debugger, ll_v, error)
     /* The error that envoked the debugger, if any. */
  ll_type_def_slot(debugger, ll_v, top_of_ar_stack)
     /* The top message this debugger can see. */
  ll_type_def_slot(debugger, ll_v, current_ar)
     /* The current message. */
  ll_type_def_slot(debugger, ll_v, exit)
     /* The <catch> to stop the debugger's interaction loop. */
  ll_type_def_slot(debugger, ll_v, level)
     /* The debugger level. */
ll_type_def_slots_end(debugger)
ll_type_def_end(debugger)


/******************************************************************/

#ifndef __ll_runtime_type_h_SKIP
#include "ll/runtime_type.h"
#endif

/******************************************************************/


