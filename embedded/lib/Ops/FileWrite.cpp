#include "FileWrite.h"

namespace ops {
    bool FileWrite::Execute() {
        if(!GetKey()) { return false; }
        
        if(!GetFileName()){ return false; }

        if(!GetFileLength()) {return false; }

        if(!WriteFile()){ return false; }
        return true;
    }

    Operation * FileWrite::Clone() {
        return new FileWrite;
    }

    bool FileWrite::GetFileLength() {

        char _file_length[4];
        Serial.readBytes(_file_length, 4);

        //Each of the 4 bytes for converting _file_length to an 32bit int
        const uint8_t b0 = _file_length[0],
                      b1 = _file_length[1], 
                      b2 = _file_length[2], 
                      b3 = _file_length[3];
        int file_length = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;

        fileLength = file_length;

        return true;
    }

    //TODO update this so only flush at 512 or end
    bool FileWrite::WriteFile() {
        
        SdFat SD;
        //Open SD card here so that we throw error and not have to restart teensy. just exists this loop.
        if (!SD.begin(CHIPSELECT, SD_SCK_MHZ(50))) { return false; }
        Serial.write(ACK);

        File myFile = SD.open(fileName, O_CREAT | O_WRITE | O_APPEND);
        while(!myFile);

        //Can reuse buffer without reinitializing as we will always overwrite the entire 64 bytes
        //EXCEPT for when file_length < 64, but even then we only write the correct amount of the
        // buffer into the file as seen in the below section
        char buffer[64];
        while( fileLength - 64 >= 0) {
            fileLength -= 64;
            
            Serial.readBytes(buffer,64);
            Serial.write(ACK);
            myFile.write(buffer, 64);
            myFile.flush();
        }

        //Once the file is less than 64, do the same thing as above
        // but only with the specific amount of bytes
        //Also need to make sure the file_length is not zero,
        //If it is zero, then none of this is necessary
        if(fileLength != 0){
            Serial.readBytes(buffer, fileLength);
            Serial.write(ACK);
            myFile.write(buffer, fileLength);
            
            //Flush to send the bytes to the SD. This could be made better as
            //It flushes every 64 bytes, but the buffer can hold 512 bytes
            //Flushing is slow so would be worth it to wait to flush
            //TODO: Only flush once buffer is full (512 bytes);
            myFile.flush(); 
        }
        myFile.close();  

        return true;
    }
}