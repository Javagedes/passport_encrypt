#pragma once
#include <Arduino.h>
#include <SdFat.h>

#define ACK 1
#define ERROR 2
#define CHIPSELECT 10

namespace ops {
    //Bytes:  1  
    //       ----
    //       |ID|
    //       ----
    class Operation {
    public:
        virtual bool Execute() = 0;
        virtual Operation * Clone() = 0;
    private:
    };
}
