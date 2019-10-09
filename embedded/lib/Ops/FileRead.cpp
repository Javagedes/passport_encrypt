#include "FileRead.h"

namespace ops {
    bool FileRead::Execute() {
        if(!GetKey()) { return false; }
        
        if(!GetFileName()){ return false; }

        if(!ReadFile()){ return false; }
        return true;
    }

    Operation * FileRead::Clone() {
        return new FileRead;
    }

    //TODO: Read and send
    bool FileRead::ReadFile() {
        
        return true;
    }
}