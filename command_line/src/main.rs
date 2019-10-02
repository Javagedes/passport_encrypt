use serialport::{available_ports, open, SerialPortSettings, SerialPort};
use std::io::{Read, Write, Cursor};
use serialport::windows::COMPort;
use std::thread::sleep;
use std::time::Duration;
use bytes::{BytesMut, BufMut};
use std::fs::File;
use crate::parser::{FileBuffer};
use serialport::ClearBuffer::All;

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

    let filename = String::from("test.txt");
    let mut file = File::open(&filename).unwrap();
    let mut buffer = Vec::new();
    let key: [u8; 16] = [
        0, 1, 2, 3,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15
    ];

    //let amount = file.read_to_end(&mut buffer).unwrap();

    let obj = FileBuffer::new(filename, key, buffer);

    port.write(&obj.encrypt_start_buffer());

    let mut buffer: Vec<u8> = Vec::new();

    //sleep(Duration::from_secs(1));
    //while port.read_data_set_ready().unwrap() == true {}
    while port.bytes_to_read().unwrap() == 0 {};
    port.read_to_end(&mut buffer);

    if buffer.pop().unwrap() == 1 {
        println!("Message acknowledged");
    }

    // let z: Vec<_> = buffer.drain(..len as usize).collect();

}
