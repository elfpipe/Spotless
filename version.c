#include "version.h"

#define VERSTAG "\0$VER: Spotless " SPOTLESS_VERSION " " __DATE__ " Live in absolute pleasure." 
#include <exec/types.h>

STRPTR USED ver = (STRPTR)VERSTAG; 