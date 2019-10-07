use std::io::Write;
use crate::parser::{Message, ACK, ERROR};
use byteorder::ReadBytesExt;
use serialport::SerialPort;
use std::env;
use std::process::exit;

mod parser;

trait Ack {
    fn ack(&mut self)->bool;
}

//TODO return String read from buffer for error reason!
impl Ack for Box<dyn SerialPort> {
    //Waits until there is something to read.
    fn ack(&mut self)->bool {
        while self.bytes_to_read().unwrap() == 0 {};
        let response = self.read_u8().unwrap();

        if response == ACK { return true; }
        else if response == ERROR { return false; }
        else {return false; }
    }
}

//TODO get file from SD card
fn get_handler()->bool {true}

//Puts a file on the SD card
//TODO require password. send password to device, hash it and see if it matches. If it matches, device will send an OK and pc will start sending the file buffer over
fn put_handler(filename: String)->bool {
    //Open the port then let the serial port know we are ready to write data;
    //TODO handle dynamically finding the port
    let mut port = serialport::open("COM3").unwrap();
    port.write_data_terminal_ready(true);
    //TODO get password from the user
    //TODO Send password over the wire for verification
    let password: String = String::from("password");

    //Create the Message we will be sending
    //This object provides multiple methods to make sending the message easier
    //TODO the methods a trait rather than special to object. will need more objects that do basically the same thing, except for receiving data, setting password, etc
    let mut obj = Message::new(filename, password);
    let mut buffer: Vec<u8> = Vec::new();

    //Create the header message, send it, wait for an Ack
    obj.file_send_header(&mut buffer);
    port.write(&mut buffer);
    if !port.ack() { println!("Failed Ack @ write header"); return false; }

    //Reuse the buffer, drain the buffer containing the file and write it to the sd.else
    //Currently, Serial buffer length on arduino is only 64 bytes
    //Wait for an ack before continuing
    //Function returns 0 when internal file buffer is finally empty
    //TODO look into doing a do-while statement here so a final write all is not necessary?
    while obj.drain_buffer(64, &mut buffer ) {
        port.write_all(&mut buffer);
        if !port.ack() { println!("Failed Ack @ write buffer"); return  false; }
    }
    //must write it one more time with what was left of the buffer
    port.write_all(&mut buffer);
    if !port.ack() { println!("Failed Ack @ Final buffer write"); return false; }
    println!("File should be written to sd...");
    return true;
}

//TODO Set the password. Delete all files on SD if you are resetting it
fn set_handler() {println!("Running set handler")}

//TODO Show all files on the SD card
fn list_handler() {}

fn main() {
    println!("Hello, world!");

// Use this to see what port it is on somehow
//    let x = available_ports().unwrap();
//
//    for i in x {
//        println!("{}", i.port_name);
//        println!("{:?}", i.port_type);
//    }

    let args: Vec<String> = env::args().collect();
    //TODO replace unwrap to state there needs to be input if it errors
    let command = args.get(1).unwrap().to_uppercase();


    if command == String::from("SET") {
        set_handler();
    }
    else if command == String::from("PUT") {
        put_handler(args.get(2).unwrap().parse().unwrap());
    }
    else if command == String::from("GET") {
        get_handler();
    }
    else if command == String::from("LIST") {
        list_handler();
    }
    else {
        println!("ERROR: Unrecognized command {}", command);
        exit(0);
    }
}

//TODO Once we have get functions working, these tests will put & get then compare the two
//For right now, one must check the files manually on the SD card
//cargo test -- --test-threads=1
#[cfg(test)]
mod tests {
    use crate::put_handler;

    #[test]
    fn put_single_line_file() {
        assert!(put_handler(String::from("single_line.txt")), true);

    }
    #[test]
    fn put_multi_line_file() {
        assert!(put_handler(String::from("multi_line.txt")), true);
    }
}
