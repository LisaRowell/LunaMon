#include <Arduino.h>

#include "PlacementNew.h"

void * operator new (__attribute__((unused)) size_t size, void *ptr) {
    return ptr;
}
