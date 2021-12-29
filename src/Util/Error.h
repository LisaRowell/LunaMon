#ifndef ERROR_H
#define ERROR_H

extern void fatalError(String errorMsg) __attribute__((noreturn));
extern void errorExit() __attribute__((noreturn));

#endif
