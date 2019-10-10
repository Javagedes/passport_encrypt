use serialport::SerialPort;
use byteorder::ReadBytesExt;
pub mod put;
pub mod get;

pub const ACK:        u8 = 1;
pub const ERROR:      u8 = 2;
const SEND_FILE:      u8 = 3;
const REQUEST_FILE:   u8 = 4;

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