#include "stack_internal.h"

/* undefined because more than one header may be included so they may ge re-defined,
   the preprocessor doesn't like redifining macros, undefining them prevents the issue. */
/* The CAT# and CAT#_kernel macros do not change, so they can stay */
#undef STACK_T
#undef STACK_NODE_T
#undef TYPE
#undef NAME
