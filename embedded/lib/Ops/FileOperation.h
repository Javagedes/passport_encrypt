#pragma once

#include "Operation.h"

namespace ops {

    //Bytes:  1   16       1             X
    //       ---------------------------------
    //       |ID|Key|filename_length|filename|
    //       ---------------------------------
    class FileOperation : public Operation {
    public:
        virtual bool Execute() = 0;
        virtual Operation * Clone() = 0;


    protected:
        uint8_t key[16];
        String fileName;
        String convertToString(char* a, int size);
        
        bool GetKey();
        bool GetFileName();
    };
}