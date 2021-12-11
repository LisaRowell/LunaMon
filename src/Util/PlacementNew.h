#ifndef PLACEMENT_NEW_H
#define PLACEMENT_NEW_H

void * operator new (__attribute__((unused)) size_t size, void *ptr) {
    return ptr;
}

#endif