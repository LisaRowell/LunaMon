#ifndef ERROR_H
#define ERROR_H

#include <etl/string.h>

using etl::istring;

extern void fatalError(const char *errorMsg) __attribute__((noreturn));
extern void fatalError(const istring &errorMsg) __attribute__((noreturn));
extern void errorExit() __attribute__((noreturn));

#endif
