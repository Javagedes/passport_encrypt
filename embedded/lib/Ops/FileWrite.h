#pragma once
#include "FileOperation.h"

namespace ops {

//Bytes:  1   16       1             X          4
//       ---------------------------------------------
//       |ID|Key|filename_length|filename|file_length|
//       ---------------------------------------------
    class FileWrite: public FileOperation {
    public:
        bool Execute() override;
        Operation * Clone() override;
        
    protected:
        int fileLength;

        bool WriteFile();
        bool GetFileLength();
    };
}