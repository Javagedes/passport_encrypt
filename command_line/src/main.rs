use serialport::{available_ports, open, SerialPortSettings, SerialPort};
use std::io::{Read, Write, Cursor};
use serialport::windows::COMPort;
use std::thread::sleep;
use std::time::Duration;
use bytes::{BytesMut, BufMut};
use std::fs::File;
use crate::parser::{FileBuffer};
use serialport::ClearBuffer::All;
use byteorder::ReadBytesExt;

mod parser;

fn main() {
    println!("Hello, world!");
// Use this to see what port it is on somehow
//    let x = available_ports().unwrap();
//
//    for i in x {
//        println!("{}", i.port_name);
//        println!("{:?}", i.port_type);
//    }
//
    let mut port = serialport::open("COM3").unwrap();
    port.write_data_terminal_ready(true);

    let filename = String::from("pest.txt");
    let mut file = File::open(&filename).unwrap();
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer);

    let key: [u8; 16] = [
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15
    ];

    let obj = FileBuffer::new(filename, key, buffer);

    port.write(&obj.encrypt_start_buffer());

    let mut response:u8 = 0;
    println!("Waiting to read...");
    while port.bytes_to_read().unwrap() == 0 {};
    response = port.read_u8().unwrap();
    if response == 1 {
        println!("Message acknowledged");
    }
    else {
        println!("Not Acknowledged");
    }

    //let mut buffer = Vec::new();
    let mut size = 0;
    while port.bytes_to_read().unwrap() == 0 {};
    //size = port.bytes_to_read().unwrap();
    //println!("{:?}", buffer);
    //println!("{}", size);

    //let mut buffer = Vec::new();
    let mut mystr = String::new();
    port.read_to_string(&mut mystr);
    println!("{}", mystr);
    // let z: Vec<_> = buffer.drain(..len as usize).collect();

}
