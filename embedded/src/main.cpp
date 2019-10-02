#include <Arduino.h>
#include <SPI.h>
#include <SdFat.h>
#include "structures.h"
#include "encrypt.h"
#include "decrypt.h"

File myFile;
File root;


const int chipSelect = 10;

void read_buffer() {

  char header[1];
  char size[1];

  digitalWrite(13, HIGH);
  while(Serial.available() == 0);
  digitalWrite(13, LOW);
  Serial.readBytes(header, 1);

  if(header[0] == 2) {
    digitalWrite(13, LOW);
    Serial.readBytes(size, 1);
    char buffer[size[0]];
    Serial.readBytes(buffer, size[0]);

    Serial.write(1);
  }

}

void encrypt_file(uint8_t key[16], File * file_in, File * file_out) {
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
}

SdFat SD;
uint8_t buffer[1024];

uint8_t key[16] = {
    1, 2, 3, 4,
    5, 6, 7, 8,
    9, 0, 1, 2,
    3, 4, 5, 6
  };


void setup() {
  
  pinMode(13, OUTPUT);
  Serial.begin(9600);
  while (!Serial);
  
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
 
 read_buffer();
  

}

