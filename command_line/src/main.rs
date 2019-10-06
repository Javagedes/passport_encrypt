use std::io::{Read, Write};
use std::fs::File;
use crate::parser::{Message, ACK, END};
use byteorder::{ReadBytesExt, WriteBytesExt};
use serialport::{SerialPortSettings, SerialPort};

mod parser;

trait Ack {
    fn ack(&mut self)->bool;
}

impl Ack for Box<dyn SerialPort> {
    //Waits until there is something to read.
    fn ack(&mut self)->bool {
        while self.bytes_to_read().unwrap() == 0 {};
        let response = self.read_u8().unwrap();

        if response == ACK { return true }
        else { return false }
    }
}

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
    let password: String = String::from("password");

    let mut obj = Message::new(filename, password);

    let mut buffer: Vec<u8> = Vec::new();

    obj.file_send_header(&mut buffer);
    port.write(&mut buffer);
    if !port.ack()
    {
        println!("Failed Ack @ write header");
    }
    while obj.drain_buffer(64, &mut buffer ) {
        port.write_all(&mut buffer);
        if !port.ack() {
            println!("Failed Ack @ write buffer");
        }
    }
    port.write_all(&mut buffer);
    port.write_u8(END);
    println!("File should be written to sd...");




}
