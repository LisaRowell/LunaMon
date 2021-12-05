#ifndef DATA_MODEL_ELEMENT_H
#define DATA_MODEL_ELEMENT_H

#include "DataModelSubscriber.h"

const unsigned maxDataModelSubscribers = 5;

class DataModelElement {
  private:
    const char *name;

  public:
    DataModelElement(const char *name);
};

#endif