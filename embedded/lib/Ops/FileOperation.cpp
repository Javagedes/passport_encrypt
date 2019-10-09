#include "FileOperation.h"

namespace ops {
    bool FileOperation::GetKey() {
        char _key[16];
        Serial.readBytes(_key, 16);
        memcpy(key, _key, 16);

        return true;
    }

    bool FileOperation::GetFileName() {
        char _fileName_length[1];
        Serial.readBytes(_fileName_length, 1);
        int fileName_length = _fileName_length[0];

        //Read in the filename as a char array. Will convert to String
        char _fileName[fileName_length];
        Serial.readBytes(_fileName, fileName_length);
        fileName = convertToString(_fileName, fileName_length);

        return true;
    }

    String FileOperation::convertToString(char* a, int size) { 
        int i; 
        String s = ""; 
        for (i = 0; i < size; i++) { 
            s = s + a[i]; 
        } 
        return s; 
    }
}