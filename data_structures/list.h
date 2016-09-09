#include "list_internal.h"

/* undefined because more than one queue header may be included so they may ge re-defined,
   the preprocessor doesn't like redifining macros, undefining them prevents the issue. */
/* The CAT# and CAT#_kernel macros do not change, so they can stay */
#undef LIST_T
#undef TYPE
#undef NAME
