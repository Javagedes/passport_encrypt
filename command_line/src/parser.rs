use byteorder::{WriteBytesExt, LittleEndian, BigEndian};
use std::io::{Write, Read};
use std::fs::File;

pub const ACK:        u8 = 1;
pub const END:        u8 = 2;
const SEND_FILE:      u8 = 3;
const REQUEST_FILE:   u8 = 4;

pub struct Message{
    filename: String,
    key: [u8; 16],
    buffer: Vec<u8>
}


impl Message {
    pub fn new(filename: String, password: String)->Message {
        let mut file = File::open(&filename).unwrap();
        let mut buffer = Vec::new();

        //TODO: Do some magic to turn a password into a key!
        let key: [u8; 16] = [
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15
        ];

        file.read_to_end(&mut buffer);

        return Message {
            filename,
            key,
            buffer
        }
    }

    //Clears the buffer first, for reuse.
    //Creates the header buffer to be send to hardware to signify that it will be
    //sending the file buffer next.
    pub fn file_send_header(&self, buf: &mut Vec<u8>)-> bool {
        let filename_size: u8 = self.filename.len() as u8;
        buf.clear();

        buf.write_u8(SEND_FILE);
        buf.write_all(&self.key);
        buf.write_u32::<BigEndian>(self.buffer.len() as u32);
        buf.write_u8(filename_size);
        buf.write_all(self.filename.as_bytes());
        return true;
    }

    //Returns true if buffer is not empty
    //Returns false if the buffer is empty
    //
    pub fn drain_buffer(&mut self, bytes: usize, buf: &mut Vec<u8>) ->bool {

        if self.buffer.len() > bytes {
            let x: Vec<u8> = self.buffer.drain(..bytes).collect();
            buf.clone_from(&x);
            return true;
        }
        else {
            let x: Vec<u8> = self.buffer.drain(..self.buffer.len()).collect();
            buf.clone_from(&x);
            return false;
        }
    }

    //Unfortunatly the Serial buffer is only 64Bytes in arduino
    //The encrypter on the arduino encryptes 16 bytes at a time
    //Therefore we could send 4 blocks at a time.
    //Normally we would send a header_id, but since it would mean we couldn't send an entire block
    //We will not send a header id, and when the arduino receives the encrypt_start_buffer,
    //The hardware will just assume the next bytes coming in contain the file.
    //pub fn encrypt_file_buffer()-> Vec<u8>
}