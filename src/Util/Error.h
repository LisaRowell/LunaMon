#ifndef ERROR_H
#define ERROR_H

#include <etl/string.h>

extern void fatalError(const char *errorMsg) __attribute__((noreturn));
extern void fatalError(const etl::istring &errorMsg) __attribute__((noreturn));
extern void errorExit() __attribute__((noreturn));

#endif
