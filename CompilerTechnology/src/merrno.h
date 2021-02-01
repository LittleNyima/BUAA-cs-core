#ifndef MERRNO_H
#define MERRNO_H

#include <string>

typedef std::string Errno;

#define E_ILLEGALSYMBOL   "a"
#define E_REDEFINITION    "b"
#define E_UNDEFINITION    "c"
#define E_ARGNUMMISMATCH  "d"
#define E_ARGTYPEMISMATCH "e"
#define E_JUDGETYPEWRONG  "f"
#define E_VOIDFUNCRETURNS "g"
#define E_ILLEGALRETURN   "h"
#define E_ILLEGALINDEX    "i"
#define E_ASSIGNTOCONST   "j"
#define E_EXPECTSEMICN    "k"
#define E_EXPECTPARENT    "l"
#define E_EXPECTBRACK     "m"
#define E_ILLEGALARRAY    "n"
#define E_ILLEGALCONST    "o"
#define E_DEFAULTMISSING  "p"

#endif // !MERRNO_H
