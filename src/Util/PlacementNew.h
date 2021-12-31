#ifndef PLACEMENT_NEW_H
#define PLACEMENT_NEW_H

#include <Arduino.h>

extern void * operator new (__attribute__((unused)) size_t size, void *ptr);

#endif
