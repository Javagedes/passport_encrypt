#pragma once
#include "Operation.h"

#define WRITE_FILE 3
#define READ_FILE 4

namespace ops {
    class Factory {
    public:
        static Operation * Create();
    private:
        static Operation * prototypes[2];
    };
}