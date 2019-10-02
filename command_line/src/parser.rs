use bytes::{BytesMut, BufMut};
use byteorder::WriteBytesExt;
use std::io::Write;

const ACK:            u8 = 1;
const ENCRYPT_HEADER: u8 = 2;
const ENCRYPT_FILE:   u8 = 3;

pub struct FileBuffer{
    filename: String,
    key: [u8; 16],
    buffer: Vec<u8>
}


impl FileBuffer {
    pub fn new(filename: String, key: [u8; 16], buffer: Vec<u8>)->FileBuffer {

        return FileBuffer{
            filename,
            key,
            buffer
        };
    }

    pub fn encrypt_start_buffer(&self)-> Vec<u8> {
        let mut buffer = Vec::new();
        let total_size: u8 = 16 + self.filename.len() as u8;

        buffer.write_u8(ENCRYPT_HEADER);
        buffer.write_u8(total_size);
        buffer.write_all(&self.key);
        buffer.write_all(self.filename.as_bytes());

        return buffer;
    }

    //Unfortunatly the Serial buffer is only 64Bytes in arduino
    //The encrypter on the arduino encryptes 16 bytes at a time
    //Therefore we could send 4 blocks at a time.
    //Normally we would send a header_id, but since it would mean we couldn't send an entire block
    //We will not send a header id, and when the arduino receives the encrypt_start_buffer,
    //The hardware will just assume the next bytes coming in contain the file.
    pub fn encrypt_file_buffer()-> Vec<u8>
}