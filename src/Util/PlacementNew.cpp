#include "PlacementNew.h"

#include <Arduino.h>

void * operator new (__attribute__((unused)) size_t size, void *ptr) {
    return ptr;
}
