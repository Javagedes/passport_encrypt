#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include "structures.h"
#include "encrypt.h"
#include "decrypt.h"

#define ACK 1
#define ERROR 2
#define RECEIVE_FILE 3
#define SEND_FILE 4
#define SET_PASSWORD 5
#define LIST_FILES 6

const int chipSelect = 10;

String convertToString(char* a, int size) { 
    int i; 
    String s = ""; 
    for (i = 0; i < size; i++) { 
        s = s + a[i]; 
    } 
    return s; 
} 

void send_handler() {}
void password_handler() {}
void list_handler() {}

//TODO make sure the result from readBytes() is what is expected, else return an ERROR!
void receive_handler() {
  
  //Open SD card here so that we throw error and not have to restart teensy. just exists this loop.

  //Read the key for encryption/decryption as char array
  char _key[16];
  Serial.readBytes(_key, 16);

  //Read the file length as a char array. Each byte must be read into the int.
  char _file_length[4];
  Serial.readBytes(_file_length, 4);

  //Read the fileName length. Filename can only be 255bits
  char _fileName_length[1];
  Serial.readBytes(_fileName_length, 1);
  int fileName_length = _fileName_length[0];

  //Read in the filename as a char array. Will convert to String
  char _fileName[fileName_length];
  Serial.readBytes(_fileName, fileName_length);

  //Read all of the information from the buffer
  //Send and ACK before doing any computation
  //This gives more time for the ACK to get sent, and the PC to respond-
  // This means less time this is possibly sitting idle
  SdFat SD;

  if (!SD.begin(chipSelect, SD_SCK_MHZ(50))) { 
    Serial.write(ERROR);
    return;
  }
  else {Serial.write(ACK);}

  //Each of the 4 bytes for converting _file_length to an 32bit int
  const uint8_t b0 = _file_length[0],
                b1 = _file_length[1], 
                b2 = _file_length[2], 
                b3 = _file_length[3];
  int file_length = (b0 << 24) | (b1 << 16) | (b2 << 8) | b3;

  String fileName = convertToString(_fileName, fileName_length); 

  //Open the file and wait until it is ready.
  //TODO: Look to see if file already exists. delete it if it does? Not sure yet on that functionality
  File myFile = SD.open(fileName, O_CREAT | O_WRITE | O_APPEND);
  while(!myFile);

  //convert the char array for key into a uint8_t array.
  //Doesn't do much but AES encryption algorithm expects uint8_t
  uint8_t key[16];
  memcpy(key, _key, 16);

  //Can reuse buffer without reinitializing as we will always overwrite the entire 64 bytes
  //EXCEPT for when file_length < 64, but even then we only write the correct amount of the
  // buffer into the file as seen in the below section
  char buffer[64];
  while( file_length - 64 >= 0) {
    file_length -= 64;
      
    Serial.readBytes(buffer,64);
    Serial.write(ACK);
    myFile.write(buffer, 64);
    myFile.flush();
  }

  //Once the file is less than 64, do the same thing as above
  // but only with the specific amount of bytes
  //Also need to make sure the file_length is not zero,
  //If it is zero, then none of this is necessary
  if(file_length != 0){
    Serial.readBytes(buffer, file_length);
    Serial.write(ACK);
    myFile.write(buffer, file_length);
      
    //Flush to send the bytes to the SD. This could be made better as
    //It flushes every 64 bytes, but the buffer can hold 512 bytes
    //Flushing is slow so would be worth it to wait to flush
    //TODO: Only flush once buffer is full (512 bytes);
    myFile.flush(); 
  }
  myFile.close();  
}

void request_handler() {

  char request_header[1];

  //Wait until their is data in the Serial buffer
  while(Serial.available() == 0);

  //Read the first byte of the buffer (the header)
  //Act on the specific request
  Serial.readBytes(request_header, 1);
  switch (request_header[0]) 
  {
    //If the PC is sending a file to the teensy
    case RECEIVE_FILE:
      receive_handler(); 
      break;

    //If the PC is requesting a file from the teensy
    case SEND_FILE:
      send_handler();
      break;
    
    case SET_PASSWORD:
      password_handler();
      break;
    
    case LIST_FILES:
      list_handler();
      break;
  }
}

/*void encrypt_file(uint8_t key[16], File * file_in, File * file_out) {
  Serial.print("Starting Encrypt Function...");
  uint32_t count = file_in->size();
  Serial.print("Size: "); Serial.println(count);
  uint8_t expandedKey[176];
  KeyExpansion(key, expandedKey);
  while(count != 0) {
    if(count <= 512) {
      uint8_t buffer[count];
      myFile.read(buffer, count);
      int messageLength = (count / 16 + 1) * 16;
      
      uint8_t * paddedMessage = new uint8_t[messageLength];
      for(int i = 0; i < messageLength; i++ ) {
        if (i >= count) {
          paddedMessage[i] = 0;
        }
        else {
          paddedMessage[i] = buffer[i];
        }
      }

      uint8_t * encryptedMessage = new uint8_t[messageLength];
      for (int i = 0; i < messageLength; i += 16) {
        encrypt::Encrypt(paddedMessage+i, expandedKey, encryptedMessage+i);
      }

      file_out->write(encryptedMessage, messageLength);
      count = 0;
      delete encryptedMessage;
      delete paddedMessage;
    }
    else {

      uint8_t buffer[512];
      myFile.read(buffer, 512);
      int messageLength = 512;
      
      uint8_t * encryptedMessage = new uint8_t[messageLength];
      for (int i = 0; i < messageLength; i += 16) {
        encrypt::Encrypt(buffer+i, expandedKey, encryptedMessage+i);
      }

      file_out->write(encryptedMessage, messageLength);
      count = count - 512;
      delete encryptedMessage;
    }
    file_out->flush();
  }
}

void decrypt_file(uint8_t key[16], File * file_in, File * file_out) {
  int count = file_in->size();
  uint8_t expandedKey[176];
  KeyExpansion(key, expandedKey);

  while(count != 0) {
    if(count <= 512) {

      uint8_t * decryptedMessage = new unsigned char[count];
      uint8_t buffer[count];

      file_in->read(buffer, count);

      for (int i = 0; i < count; i += 16) {
		    decrypt::Decrypt(buffer + i, expandedKey, decryptedMessage + i);
	    }

      file_out->write(decryptedMessage, count);
      count = 0;
      delete decryptedMessage;
    }
    else {
      uint8_t * decryptedMessage = new unsigned char[512];
      uint8_t buffer[512];

      file_in->read(buffer, 512);

      for (int i = 0; i < 512; i += 16) {
		    decrypt::Decrypt(buffer + i, expandedKey, decryptedMessage + i);
	    }

      file_out->write(decryptedMessage, 512);
      count -= 512;
      delete decryptedMessage;

    }
    file_out->flush();
  }
}*/

void setup() {
  
  Serial.begin(9600);
  while (!Serial);

  /*if (!SD.begin(chipSelect, SD_SCK_MHZ(50))) { 
    Serial.write(ERROR);
  }
  else {
    Serial.write(ACK);
  }*/
  
}
/*void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!SD.begin(chipSelect, SD_SCK_MHZ(100))) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("Card Initialized...");

  Serial.println("Opening File...");
  //myFile = SD.open("TESTTX~1.TXT");

  myFile = SD.open("INPUT.mp4");
  
  while(!myFile.available());
  Serial.println("File Opened...");

  Serial.println("Opening encrypted File...");
  //File eFile = SD.open("ENCRYPT.mp4", FILE_WRITE);
  File eFile = SD.open("ENCRYPT.mp4", O_CREAT | O_WRITE | O_APPEND);
  while(!eFile);
  Serial.println("File opened...");

  Serial.println("Encrypting File...");
  encrypt_file(key, &myFile, &eFile );
  Serial.println("File Encrypted...");

  myFile.close();
  eFile.close();
  File eeFile = SD.open("ENCRYPT.txt");
  File oFile = SD.open("DECRYPT.txt", FILE_WRITE);

  Serial.println("Decrypting File...");
  decrypt_file(key, &eeFile, &oFile);
  Serial.println("File Decrypted...");
  eeFile.close();
  oFile.close();
  //File oFile = SD.open("unencrypted_test.txt", FILE_WRITE);
}*/

void loop() {
 
 request_handler(); 

}

