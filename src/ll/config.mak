# $Id: config.mak,v 1.12 2008/01/07 06:05:24 stephens Exp $

# Has GNU Readline.
HAS_READLINE=1
# Has GNU Readline History.
HAS_HISTORY=1

# Enable assertions.
ll_ASSERT_ENABLE=0

# Use a machine word for ll_v.
ll_v_WORD=1

# Use inline call macros.
ll_USE_INLINE_CALL_MACROS=1

# Garbage Collection:
ll_USE_GC=1
ll_USE_GC_BOEHM=1
ll_USE_GC_TREADMILL=0

# Use a lookup cache at all call sites.
ll_USE_LCACHE=1

# Use a type-method-alist in <operation>
# instead of a op-method-alist in <type>.
ll_USE_TYPE_METH_ALIST=1
ll_USE_OP_METH_ALIST=1

# Record lookup cache stats.
ll_LOOKUP_CACHE_STATS=0

# Store C FILE and LINE information in <message> objects.
ll_USE_FILE_LINE=0

ll_BC_RUN_DEBUG=0
ll_BC_DUMP=0

# Record byte-code execution statistics.
ll_BC_METER=0

# Use global variables.
ll_GLOBAL_VARS=0

# Allow multiple systems in the same process.
ll_MULTIPLICITY=0

# Use statically linked <type> objects.
ll_TYPES_STATIC=1

