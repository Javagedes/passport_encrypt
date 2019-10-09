#include "Factory.h"
#include "FileWrite.h"
#include "FileRead.h"



namespace ops {
    Operation * Factory::Create() {
        char request_header[1];
        Serial.readBytes(request_header, 1);

        switch (request_header[0]) 
        {
            //If the PC is sending a file to the teensy
            case WRITE_FILE:
            return prototypes[0];

            //If the PC is requesting a file from the teensy
            case READ_FILE:
            return prototypes[1];

            default: return nullptr;
        }
    }

    Operation * Factory::prototypes[] = {new FileWrite, new FileRead};
}