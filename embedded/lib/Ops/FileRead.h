#pragma once
#include "FileOperation.h"

namespace ops {

//Bytes:  1   16       1             X    
//       ---------------------------------
//       |ID|Key|filename_length|filename|
//       ---------------------------------
    class FileRead : public FileOperation {
    public:
        bool Execute() override;
        Operation * Clone() override;

    protected:
        bool ReadFile();
    };
}