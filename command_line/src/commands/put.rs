use crate::commands::{ACK, Ack, SEND_FILE};
use byteorder::{WriteBytesExt, BigEndian};
use std::io::{Write, Read};
use std::fs::File;

pub struct Put {
    id: u8,
    key: [u8; 16],
    filename: String,
    file_length: u32,
    buffer:  Vec<u8>
}

impl Put {
    pub fn new(id: u8, filename: String, password: String)->Put {

        //TODO: Do some magic to turn a password into a key!
        let key: [u8; 16] = [
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15
        ];

        let mut file = File::open(filename.clone()).unwrap();
        let mut buffer: Vec<u8> = Vec::new();
        let file_length = file.read_to_end(&mut buffer).unwrap() as u32;

        return Put {
            id,
            key,
            filename,
            file_length,
            buffer
        }
    }

    pub fn header_buffer(&self, buffer: &mut Vec<u8>) {
        buffer.write_u8(self.id).unwrap();
        buffer.write_all(&self.key).unwrap();
        buffer.write_u8(self.filename.len() as u8).unwrap();
        buffer.write_all(self.filename.as_bytes()).unwrap();
        buffer.write_u32::<BigEndian>(self.file_length).unwrap();
    }

    pub fn drain_file_buffer(&mut self,bytes: usize, buffer: &mut Vec<u8>)->bool{
        if self.buffer.len() > bytes {
            let x: Vec<u8> = self.buffer.drain(..bytes).collect();
            buffer.clone_from(&x);
            return true;
        }
        else {
            let x: Vec<u8> = self.buffer.drain(..self.buffer.len()).collect();
            buffer.clone_from(&x);
            return false;
        }
    }
}

//Puts a file on the SD card
//TODO require password. send password to device, hash it and see if it matches. If it matches, device will send an OK and pc will start sending the file buffer over
pub fn put_handler(filename: String)->bool {
    //Open the port then let the serial port know we are ready to write data;
    //TODO handle dynamically finding the port
    let mut port = serialport::open("COM3").unwrap();
    port.write_data_terminal_ready(true).unwrap();
    //TODO get password from the user
    //TODO Send password over the wire for verification
    let password: String = String::from("password");

    //Create the Message we will be sending
    //This object provides multiple methods to make sending the message easier
    //TODO the methods a trait rather than special to object. will need more objects that do basically the same thing, except for receiving data, setting password, etc
    let mut put_obj = Put::new(SEND_FILE, filename, password);
    let mut buffer: Vec<u8> = Vec::new();

    //Create the header message, send it, wait for an Ack
    put_obj.header_buffer(&mut buffer);
    port.write(&mut buffer).unwrap();
    if !port.ack() { println!("Failed Ack @ write header"); return false; }

    //Reuse the buffer, drain the buffer containing the file and write it to the sd.else
    //Currently, Serial buffer length on arduino is only 64 bytes
    //Wait for an ack before continuing
    //Function returns 0 when internal file buffer is finally empty
    //TODO look into doing a do-while statement here so a final write all is not necessary?
    while put_obj.drain_file_buffer(64, &mut buffer ) {
        port.write_all(&mut buffer).unwrap();
        if !port.ack() { println!("Failed Ack @ write buffer"); return  false; }
    }
    //must write it one more time with what was left of the buffer
    port.write_all(&mut buffer).unwrap();
    if !port.ack() { println!("Failed Ack @ Final buffer write"); return false; }
    println!("File should be written to sd...");
    return true;
}
